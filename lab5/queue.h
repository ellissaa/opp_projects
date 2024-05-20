#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include <pthread.h>

#include "task.h"

typedef struct {
    TaskT* tasks;
    int capacity;
    int stored;
    int end_index;
    pthread_mutex_t mutex;
} QueueT;

void CreateQueue(QueueT* queue, int capacity);

void DestroyQueue(QueueT* queue);

void QueueAdd(QueueT* queue, TaskT task);

TaskT* QueueGet(QueueT* queue, TaskT* task);

int QueueIsEmpty(const QueueT* queue);

void QueueBlock(QueueT* queue);

void QueueUnblock(QueueT* queue);

#endif