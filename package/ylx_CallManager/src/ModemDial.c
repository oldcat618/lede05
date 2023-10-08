#include "ModemDial.h"
#include "ModemDetect.h"
#include <fros_common.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


FrosQueue queue;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int update_dial_config(dial_info_t *dial)
{
    if (dial == NULL) {
        return 1;
    }
    pthread_mutex_lock(&mutex);
    fros_en_queue(&queue, (void *)dial);
    pthread_mutex_unlock(&mutex);

    printf("update dial config\n");

    return 0;
}

void *queueEventDetect(void *arg) {
    FrosQueue* queue = (FrosQueue*)arg;
    dial_info_t* dat = NULL;;
    // struct timespec sleepTime;
    // sleepTime.tv_sec = 3;
    // sleepTime.tv_nsec = 0;

    while (1) {
        // nanosleep(&sleepTime, NULL);

        pthread_mutex_lock(&mutex);

        if (!fros_queue_empty(queue)) {
            dat = (dial_info_t*)fros_de_queue(queue);
            if (dat) {
                if (!strstr(dat->dialcmd, DIALTOOL_ATCMD)) {
                    system(dat->dialcmd);
                    break;
                }
            } 
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


void init_dial_config()
{
    fros_init_queue(&queue, 0);
     if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("mutex init");
        exit(EXIT_FAILURE);
    }

    pthread_t queue_idt;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&queue_idt, &attr, queueEventDetect, &queue) != 0) {
        perror("pthread create");
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&attr);
}

