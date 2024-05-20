#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "utility.h"

void CreateQueue(QueueT* queue, int capacity) {
    queue->tasks = malloc(capacity * sizeof(TaskT));
    if (queue->tasks == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int ret_code = pthread_mutex_init(&queue->mutex, NULL);
    if (ret_code != 0) {
        HandlePthreadError("mutex_init", ret_code);
    }

    queue->capacity = capacity;
    queue->stored = 0;
    queue->end_index = 0; // начало очереди
}

void DestroyQueue(QueueT* queue) {
    free(queue->tasks);
    int ret_code = pthread_mutex_destroy(&queue->mutex);
    if (ret_code != 0) {
        HandlePthreadError("pthread_mutex_destroy", ret_code);
    }
}

void QueueAdd(QueueT* queue, TaskT task) {
    QueueBlock(queue);

    if (queue->stored == queue->capacity) {
        fprintf(stderr, "The queue is full\n");
        exit(EXIT_FAILURE);
    }

    queue->tasks[queue->end_index] = task;
    queue->stored++;
    queue->end_index = (queue->end_index + 1) % queue->capacity; // закольцевали

    QueueUnblock(queue);
}

TaskT* QueueGet(QueueT* queue, TaskT* task) {
    QueueBlock(queue);

    if (queue->stored == 0) {
        QueueUnblock(queue);
        return NULL;
    }

    int start_index = (queue->end_index - queue->stored + queue->capacity) % queue->capacity;
    queue->stored--;
    *task = queue->tasks[start_index];

    QueueUnblock(queue);
    return task;
}

int QueueIsEmpty(const QueueT* queue) {
    return queue->stored == 0;
}

void QueueBlock(QueueT* queue) {
    int ret_code = pthread_mutex_lock(&queue->mutex);
    if (ret_code != 0) {
        HandlePthreadError("mutex_lock", ret_code);
    }
}

void QueueUnblock(QueueT* queue) {
    int ret_code = pthread_mutex_unlock(&queue->mutex);
    if (ret_code != 0) {
        HandlePthreadError("mutex_unlock", ret_code);
    }
}