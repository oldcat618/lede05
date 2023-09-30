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

#define DEFAULT(item) "modem_dial.default."#item

int main()
{
    char *path = "/sys/devices/platform/1e1c0000.xhci/usb2/2-1";

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
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(path), buf, sizeof(buf)))
        default_conf.path = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(custom), buf, sizeof(buf)))
        default_conf.custom = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(apn), buf, sizeof(buf)))
        default_conf.apn = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(user), buf, sizeof(buf)))
        default_conf.user = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(passwd), buf, sizeof(buf)))
        default_conf.passwd = strdup(buf);
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(auth), buf, sizeof(buf)))
        default_conf.auth = atoi(buf);
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(ipv4), buf, sizeof(buf)))
        default_conf.ipv4 = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(ipv6), buf, sizeof(buf)))
        default_conf.ipv6 = atoi(buf)?1:0;
    if (UCI_OK == fros_uci_get_value(ctx, DEFAULT(pincode), buf, sizeof(buf)))
        default_conf.pincode = atoi(buf);

    return 0;
}