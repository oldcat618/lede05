
/*
 *   Copyright (C) 2023 Derry <destan19@126.com>
**/
#ifndef __FROS_QUEUE_H__
#define __FROS_QUEUE_H__

typedef struct FrosQNode{
    void *data;
    struct FrosQNode * next;
}FrosQNode;

typedef struct FrosFrosQueue{
    struct FrosQNode * top;
    struct FrosQNode * rear;
    int size;
    int const_data;
}FrosQueue;

typedef int (*fros_queue_iter_func)(void *arg, void *data);

int fros_queue_empty(FrosQueue *q);
void fros_init_queue(FrosQueue *q, int const_data);
int fros_queue_size(FrosQueue *q);
void fros_destroy_queue(FrosQueue *q);
void fros_clear_queue(FrosQueue *q);
void fros_en_queue(FrosQueue *q, void *data);
void *fros_de_queue(FrosQueue *q);
int fros_merge_queue(FrosQueue *dq, FrosQueue *sq);
void fros_queue_foreach(FrosQueue *q, void *arg, fros_queue_iter_func iter);
#endif