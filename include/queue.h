/**
 * Blocking concurrent queue implementation
 * Based on: https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct _queue_item_t
{
    void *data;
    struct _queue_item_t *next;
} queue_item_t;

typedef struct
{
    queue_item_t *back;
    pthread_mutex_t back_mutex;
    queue_item_t *front;
    pthread_mutex_t front_mutex;
} queue_t;

queue_t *queue_init()
{
    queue_t *queue = malloc(sizeof(queue_t));

    queue->back = NULL;
    pthread_mutex_init(&queue->back_mutex, PTHREAD_MUTEX_NORMAL);
    queue->front = NULL;
    pthread_mutex_init(&queue->front_mutex, PTHREAD_MUTEX_NORMAL);

    queue_item_t *item = malloc(sizeof(queue_item_t));
    item->data = NULL;
    item->next = NULL;
    queue->back = item;
    queue->front = queue->back;

    return queue;
}

void queue_destroy(queue_t *queue)
{
    // TODO free remaining items?
    pthread_mutex_destroy(&queue->back_mutex);
    pthread_mutex_destroy(&queue->front_mutex);
    free(queue);
}

void queue_put(queue_t *queue, void *data)
{
    queue_item_t *item = malloc(sizeof(queue_item_t));
    item->data = data;
    item->next = NULL;

    pthread_mutex_lock(&queue->back_mutex);
    queue->back->next = item;
    queue->back = item;
    pthread_mutex_unlock(&queue->back_mutex);
}

int queue_get(queue_t *queue, void **data)
{
    pthread_mutex_lock(&queue->front_mutex);
    queue_item_t *front = queue->front;
    queue_item_t *next = front->next;
    if (next == NULL)
    {
        pthread_mutex_unlock(&queue->front_mutex);
        return false;
    }
    *data = next->data;
    queue->front = next;
    pthread_mutex_unlock(&queue->front_mutex);

    free(front);
    return true;
}

#endif
