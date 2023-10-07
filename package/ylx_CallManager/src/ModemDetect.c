#include "ModemDetect.h"
#include "ModemDial.h"
#include <fros_common.h>
#include <cjson/cJSON.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int vendor_is_compatible(int vendor);

void *hotplugEventDetect(void *arg)
{
    int fd = open(HOTPLUG_PIPE_PATH, O_RDONLY | O_CREAT, 0777);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    char result[256];
    while (1) {
        ssize_t bytesRead = read(fd, result, sizeof(result));
        if (bytesRead == 0) {
            // usleep(10000);
            continue;
        }
        
        printf("pipe: %s\n", result);
        sleep(6); //wait for module init
        update_modem_json(result);
    }
    close(fd);
    return 0;
}

int coldplugEventDetect()
{
    char result[512];
    runShellCmd(COLDPLUG_SCRIPT, result, sizeof(result));

    char *line;
    char *ptr;
    line = strtok_r((char *)result, "\n", &ptr);

    while (line != NULL) {
        if (strncmp(line, "insert", strlen("insert")) == 0) {
            update_modem_json(line);
        }
        line = strtok_r(NULL, "\n", &ptr);
    }
    return 0;
}

int update_modem_json(const char *raw)
{   
    const char *fn = MODEM_INFO_PATH;
    FILE *file = fopen(fn, "a+");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize == 0) {
        cJSON *root = cJSON_CreateObject();
        if (root == NULL) {
            fclose(file);
            fprintf(stderr, "Error creating JSON object\n");
            return 1;
        }

        cJSON_AddItemToObject(root, "modem", cJSON_CreateArray());

        char *json_str = cJSON_Print(root);
        cJSON_Delete(root);
        fileSize = strlen(json_str);
        fclose(file);
        file = fopen(fn, "w");
        fprintf(file, "%s\n", json_str);
        free(json_str);
        fclose(file);

        file = fopen(fn, "a+");
        if (file == NULL) {
            fprintf(stderr, "Unable to open file\n");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
    }

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        fprintf(stderr, "Memory error\n");
        fclose(file);
        return 1;
    }

    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    fclose(file);

    if (bytesRead != fileSize) {
        fprintf(stderr, "File reading failure\n");
        free(fileContent);
        return 1;
    }

    fileContent[fileSize] = '\0'; 
    cJSON *root = cJSON_Parse(fileContent);
    if (root == NULL) {
        const char *error = cJSON_GetErrorPtr();
        if (error != NULL) {
            fprintf(stderr, "JSON parse error: %s\n", error);
        }
        free(fileContent);
        return 1;
    }

    cJSON *modemArray = cJSON_GetObjectItem(root, "modem");
    if (modemArray == NULL || !cJSON_IsArray(modemArray)) {
        fprintf(stderr, "Invalid JSON data\n");
        cJSON_Delete(root);
        free(fileContent);
        return 1;
    }

    char *action = NULL, *idVendor = NULL, *idProduct = NULL, *interface = NULL, *atDev = NULL, *driver = NULL, *pathnode = NULL;

    if(strncmp(raw, "insert", strlen("insert")) == 0){
        int result = sscanf(raw,"%ms %ms %ms %ms %ms %ms %ms",&action, &idVendor, &idProduct, &interface, &atDev, &driver, &pathnode);
        if(result != 7) {
            return 1;
        }

        int i = cJSON_GetArraySize(modemArray) - 1;
        while (i >= 0) {
            cJSON *element = cJSON_GetArrayItem(modemArray, i);
            cJSON *node = cJSON_GetObjectItem(element, "PathNode");

            if (node != NULL) {
                if (strcmp(node->valuestring, pathnode) == 0) {
                    cJSON_DeleteItemFromArray(modemArray, i);
                }
            }
            i--;
        }

        cJSON *newModem = cJSON_CreateObject();
        cJSON_AddStringToObject(newModem, "idVendor", idVendor);
        cJSON_AddStringToObject(newModem, "idProduct", idProduct);
        cJSON_AddStringToObject(newModem, "InterFace", interface);
        cJSON_AddStringToObject(newModem, "atDev", atDev);
        cJSON_AddStringToObject(newModem, "Driver", driver);
        cJSON_AddStringToObject(newModem, "PathNode", pathnode);

        dial_conf_t dial_conf ;
        memset(&dial_conf, 0, sizeof(dial_conf));
        load_modem_dial_conf(pathnode, &dial_conf);
        char dialcmd[128]={0};
        char tmp[64];
        switch (strtol(idVendor, NULL, 16)) {
            case VENDOR_QUECTEL:
                sprintf(tmp, "%s", DIALTOOL_QUECTEL);
            break;
            case VENDOR_MEIG:
                sprintf(tmp, "%s", DIALTOOL_MEIG);
            break;
            case VENDOR_FIBOCOM:
                sprintf(tmp, "%s", DIALTOOL_FIBOCOM);
            break;
            default:
                sprintf(tmp, "%s", DIALTOOL_QUECTEL);
            break;
        }
        strcat(dialcmd, tmp);
        sprintf(tmp, " -i %s", interface);
        strcat(dialcmd, tmp);
        if(dial_conf.ipv4)
            strcat(dialcmd, " -4");
        if(dial_conf.ipv6)
            strcat(dialcmd, " -6");
        if(dial_conf.custom){
            if (strlen(dial_conf.apn)) {
                if(!strstr(dial_conf.user, "null") && 
                    !strstr(dial_conf.passwd, "null") && 
                    (dial_conf.auth > 0 && dial_conf.auth < 3)) {
                    sprintf(tmp, " -s %s %s %s %d", dial_conf.apn, dial_conf.user, dial_conf.passwd, dial_conf.auth);
                }else {
                    sprintf(tmp, " -s %s", dial_conf.apn);
                }
                strcat(dialcmd, tmp);
            }
        }

        if (dial_conf.pincode) {
            sprintf(tmp, " -p %d ", dial_conf.pincode);
            strcat(dialcmd, tmp);
        }

        strcat(dialcmd, " &");

        cJSON *newConfig = cJSON_CreateObject();
        cJSON_AddNumberToObject(newConfig, "IPV4", dial_conf.ipv4);
        cJSON_AddNumberToObject(newConfig, "IPV6", dial_conf.ipv6);
        cJSON_AddNumberToObject(newConfig, "Custom", dial_conf.custom);
        cJSON_AddStringToObject(newConfig, "APN", dial_conf.apn);
        cJSON_AddStringToObject(newConfig, "User", dial_conf.user);
        cJSON_AddStringToObject(newConfig, "PassWord", dial_conf.passwd);
        cJSON_AddNumberToObject(newConfig, "Auth", dial_conf.auth);
        cJSON_AddNumberToObject(newConfig, "PinCode", dial_conf.pincode);
        cJSON_AddStringToObject(newConfig, "DialCmd", dialcmd);
        cJSON_AddItemToObject(newModem, "dial", newConfig);
        cJSON_AddItemToArray(modemArray, newModem);

        
        if(vendor_is_compatible(strtol(idVendor, NULL, 16))){
            dial_info_t dial_info = {
                .dialcmd = strdup(dialcmd),
                .atDev = strdup(atDev),
                .path = strdup(pathnode),
                .idVendor = strtol(idVendor, NULL, 16),
                .idProduct = strtol(idProduct, NULL, 16)
            };
            
            update_dial_config(&dial_info);
        }

    }else if(strncmp(raw, "remove", strlen("remove")) == 0){
        int result = sscanf(raw,"%ms %ms %ms %ms",&action, &idVendor, &idProduct, &pathnode);
        if(result != 4) {
            return 1;
        }
        int i = cJSON_GetArraySize(modemArray) - 1;
        while (i >= 0) {
            cJSON *element = cJSON_GetArrayItem(modemArray, i);
            cJSON *node = cJSON_GetObjectItem(element, "PathNode");

            if (node != NULL) {
                if (strcmp(node->valuestring, pathnode) == 0) {
                    cJSON_DeleteItemFromArray(modemArray, i);
                }
            }
            i--;
        }
    }

    FILE *outputFile = fopen(fn, "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Unable to open file for writing\n");
        cJSON_Delete(root);
        free(fileContent);
        return 1;
    }

    char *updatedJsonStr = cJSON_Print(root);
    fputs(updatedJsonStr, outputFile);
    fclose(outputFile);
    cJSON_Delete(root);
    free(fileContent);
    free(updatedJsonStr);

    sleep(1);
    system(NETWORKUPDATE_SCRIPT);

    return 0;
}

int load_modem_dial_conf(const char *path, dial_conf_t *conf)
{
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx){
        return -1;
    }

    int index = fros_uci_get_section_index_by_str_opt(ctx, MODEM_DIAL_PACKAGE_NAME, MODEM_SECTION_NAME, "path", path);
    if (index < 0){
        uci_free_context(ctx);
        printf("not found path:%s\n", path);
        return -1;
    }

    char buf[128] = {0};
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].path", index, buf, sizeof(buf)))
        conf->path = strdup(buf);
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].path", index, buf, sizeof(buf)))
        conf->path = strdup(buf);
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].custom", index, buf, sizeof(buf)))
        conf->custom = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].apn", index, buf, sizeof(buf)))
        conf->apn = strdup(buf);
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].user", index, buf, sizeof(buf)))
        conf->user = strdup(buf);
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].passwd", index, buf, sizeof(buf)))
        conf->passwd = strdup(buf);
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].auth", index, buf, sizeof(buf)))
        conf->auth = atoi(buf);
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].ipv4", index, buf, sizeof(buf)))
        conf->ipv4 = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].ipv6", index, buf, sizeof(buf)))
        conf->ipv6 = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_array_value(ctx, "modem_dial.@dial[%d].pincode", index, buf, sizeof(buf)))
        conf->pincode = atoi(buf);

    uci_free_context(ctx);

    return 0;
}


int runShellCmd(const char *cmd, char *result, int resultSize)
{
    char buf[256];
    int len = 0;

    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("%s run error!\n", cmd);
        return 0;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        int lineLen = strlen(buf);
        if (len + lineLen < resultSize) {
            strcpy(result + len, buf);
            len += lineLen;
        } 
    }

    pclose(fp);

    if (len < resultSize) {
        result[len] = '\0';
    }
    return len;
}


static int vendor_is_compatible(int vendor)
{
    return (vendor == VENDOR_QUECTEL) 
        || (vendor == VENDOR_MEIG)
        || (vendor == VENDOR_FIBOCOM)
        ;
}