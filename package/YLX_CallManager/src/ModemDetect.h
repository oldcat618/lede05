#ifndef __MODEMDETECT_H__
#define __MODEMDETECT_H__

enum ACTION_TYPE{
    ACTION_ADD,
    ACTION_REMOVE,
};

typedef struct {
    int action;
    int bus;
    int port;
    char *path;
    char *idProduct;
    char *idVendor;
    char *interface;
    char *driver;
    char *tty;
} modem_info_t;

typedef struct {
    int action;
    char *path;
    char *product;
    char *interface;
    char *driver;
    char *devname;
} modem_raw_info_t;


#define NETLINK_MAX_PAYLOAD 2048
#define COLDPLUG_SCRIPT "/sbin/YLX_CallManager_ColdPlug.sh 2>/dev/null"
#define HOTPLUG_PIPE_PATH "/tmp/modem-ipc"
// #define HOTPLUG_PIPE_LOCK "/tmp/modem-lock"
#define MODEM_INFO_PATH "/tmp/modem.json"

void *hotplugEventDetect(void *arg);
int coldplugEventDetect();

int update_modem_json(const char *raw);
int runShellCmd(const char *cmd, char *result, int resultSize);

#endif // !__MODEMDETECT_H__
