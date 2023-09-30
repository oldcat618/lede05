#include "ModemDetect.h"
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

    coldplugEventDetect();
    sleep(1);

    pthread_t hotplugThread;

    if (pthread_create(&hotplugThread, NULL, hotplugEventDetect, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    pthread_join(hotplugThread, NULL);

    return 0;
}