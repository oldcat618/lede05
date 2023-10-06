#ifndef __MODEMDETECT_H__
#define __MODEMDETECT_H__

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

typedef struct {
    int idVendor;
    int idProduct;
    char *path;
    char *atDev;
    char *dialcmd;
} dial_info_t;

enum AUTH_TYPE {
    PAP=1,
    CHAP,
    MSCHAPV2,
};

typedef enum {
    VENDOR_QUECTEL =   0x2c7c,
    VENDOR_MEIG    =   0x2dee,
    VENDOR_FIBOCOM =   0x2cb7,
}VENDOR_TYPE_t;



#define COLDPLUG_SCRIPT "/sbin/ylx_CallManager_ColdPlug.sh 2>/dev/null"
#define HOTPLUG_PIPE_PATH "/tmp/modem-ipc"
// #define HOTPLUG_PIPE_LOCK "/tmp/modem-lock"
#define MODEM_INFO_PATH "/tmp/modem.json"
#define MODEM_DIAL_PACKAGE_NAME "modem_dial"
#define MODEM_SECTION_NAME      "dial"
#define DIALTOOL_QUECTEL    "quectel-CM"
#define DIALTOOL_MEIG       "Meig-CM"
#define DIALTOOL_FIBOCOM    "fibocom-dial"
#define DIALTOOL_ATCMD    "ATCMD"

void *hotplugEventDetect(void *arg);
int coldplugEventDetect();
int load_modem_dial_conf(const char *path, dial_conf_t *conf);
int update_modem_json(const char *raw);
int runShellCmd(const char *cmd, char *result, int resultSize);

#endif // !__MODEMDETECT_H__
