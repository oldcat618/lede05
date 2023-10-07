#include "ModemDetect.h"
#include "ModemDial.h"
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>



int main() {

    init_dial_config();
    coldplugEventDetect();
    sleep(1);

    // pthread_t hotplug_idt;
    // // pthread_attr_t attr;
    // // pthread_attr_init(&attr);
    // // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // if (pthread_create(&hotplug_idt, NULL, hotplugEventDetect, NULL) != 0) {
    //     perror("pthread create");
    //     exit(EXIT_FAILURE);
    // }

    
    // // pthread_attr_destroy(&attr);
    // pthread_join(hotplug_idt, NULL);

    return 0;
}