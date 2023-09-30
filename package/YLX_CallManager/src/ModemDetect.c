#include "ModemDetect.h"
#include "cJSON.h"
// #include <json-c/json.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        if (bytesRead == -1) {
            perror("read pipe");
            exit(1);
        } else if (bytesRead == 0) {
            usleep(10000);
            continue;
        }
        printf("pipe: %s\n", result);
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
        cJSON_AddItemToArray(modemArray, newModem);
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