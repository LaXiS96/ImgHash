/**
 * Blocking concurrent queue implementation
 * Based on: https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <threads.h>

typedef struct _queue_item_t
{
    void *data;
    struct _queue_item_t *next;
} queue_item_t;

typedef struct
{
    queue_item_t *back;
    mtx_t back_mutex;
    queue_item_t *front;
    mtx_t front_mutex;
} queue_t;

queue_t *queue_init()
{
    queue_t *queue = malloc(sizeof(queue_t));

    queue->back = NULL;
    mtx_init(&queue->back_mutex, mtx_plain);
    queue->front = NULL;
    mtx_init(&queue->front_mutex, mtx_plain);

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
    mtx_destroy(&queue->back_mutex);
    mtx_destroy(&queue->front_mutex);
    free(queue);
}

void queue_put(queue_t *queue, void *data)
{
    queue_item_t *item = malloc(sizeof(queue_item_t));
    item->data = data;
    item->next = NULL;

    mtx_lock(&queue->back_mutex);
    queue->back->next = item;
    queue->back = item;
    mtx_unlock(&queue->back_mutex);
}

int queue_get(queue_t *queue, void **data)
{
    mtx_lock(&queue->front_mutex);
    queue_item_t *front = queue->front;
    queue_item_t *next = front->next;
    if (next == NULL)
    {
        mtx_unlock(&queue->front_mutex);
        return false;
    }
    *data = next->data;
    queue->front = next;
    mtx_unlock(&queue->front_mutex);

    free(front);
    return true;
}

#endif
