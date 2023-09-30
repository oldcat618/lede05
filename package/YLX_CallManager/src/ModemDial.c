#include "ModemDial.h"
#include "fros_uci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
	char *path;
	int custom;
    char *apn;
	char *user;
	char *passwd;
	int auth;
	int ipv4;
	int ipv6;
	int pincode;
} dial_conf_t;

enum{
    PAP=1,
    CHAP,
    MSCHAPV2,
}auth_t;

#define ITEM(item) "networkmonitor.config."#item

int update_dial_config(const char *path)
{
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx){
        return NULL;
    }

    dial_conf_t default_conf = {
        .path = "null",
        .custom = 0,
        .apn = "null",
        .user = "null",
        .passwd = "null",
        .auth = 0,
        .ipv4 = 1,
        .ipv6 = 1,
        .pincode = 0
    };

    char buf[256] = {0};
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(path), buf, sizeof(buf)))
        default_conf.path = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(custom), buf, sizeof(buf)))
        default_conf.custom = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(apn), buf, sizeof(buf)))
        default_conf.apn = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(user), buf, sizeof(buf)))
        default_conf.user = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(passwd), buf, sizeof(buf)))
        default_conf.passwd = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(auth), buf, sizeof(buf)))
        default_conf.auth = atoi(buf);
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(ipv4), buf, sizeof(buf)))
        default_conf.ipv4 = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(ipv6), buf, sizeof(buf)))
        default_conf.ipv6 = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_value(ctx, ITEM(pincode), buf, sizeof(buf)))
        default_conf.pincode = atoi(buf);

    return 0;
}