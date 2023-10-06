/*
 *   Copyright (C) 2023 Derry <destan19@126.com>
**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fros_util.h"
#include "fros_queue.h"

int fros_queue_empty(FrosQueue *q)
{
    return (q->top->next == NULL);
}
void fros_init_queue(FrosQueue *q, int const_data)
{
    q->rear = q->top = (FrosQNode *)malloc(sizeof(FrosQNode));
    q->rear->next = NULL;
    q->size = 0;
    q->const_data = const_data;
}

int fros_queue_size(FrosQueue *q)
{
    return q->size;
}

void fros_destroy_queue(FrosQueue *q)
{
    if (!q)
        return;
    while (!fros_queue_empty(q))
    {
        void *data = fros_de_queue(q);
        if (!q->const_data)
        {
            free(data);
        }
    }
    free(q->top);
    q->rear = q->top = NULL;
    q->size = 0;
}

void fros_clear_queue(FrosQueue *q)
{
    if (!q)
        return;
    while (!fros_queue_empty(q))
    {
        void *data = fros_de_queue(q);
        if (!q->const_data)
        {
            free(data);
        }
    }
    q->size = 0;
}

void fros_en_queue(FrosQueue *q, void *data)
{
    if (NULL == q || NULL == q->rear || NULL == q->top)
    {
        printf("error, queue is NULL\n");
        return;
    }
    FrosQNode *node = (FrosQNode *)malloc(sizeof(FrosQNode));
    node->next = NULL;
    node->data = data;
    q->rear->next = node;
    q->rear = node;
    q->size++;
}

void *fros_de_queue(FrosQueue *q)
{
    if (NULL == q || NULL == q->rear || NULL == q->top)
    {
        printf("error, queue is NULL\n");
        return;
    }

    if (q->top->next == NULL)
    {
        printf("queue is empty\n");
        return NULL;
    }

    FrosQNode *node = q->top->next;
    q->top->next = node->next;

    if (q->rear == node)
    {
        q->rear = q->top;
    }
    void *data = node->data;
    free(node);
    q->size--;
    return data;
}

int fros_merge_queue(FrosQueue *dq, FrosQueue *sq)
{
    while (!fros_queue_empty(sq))
    {
        fros_en_queue(dq, fros_de_queue(sq));
    }
    return dq->size;
}

void fros_queue_foreach(FrosQueue *q, void *arg, fros_queue_iter_func iter)
{
    if (fros_queue_empty(q))
    {
        printf("queue is empty");
        return;
    }
    struct FrosQNode *p = q->top->next;
    while (p != NULL)
    {
        iter(arg, p->data);
        p = p->next;
    }
}