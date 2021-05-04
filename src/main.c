#if defined(__STDC_NO_THREADS__)
#error Support for C11 threads is required
#endif

#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <pHash_c.h>

#include "queue.h"

#define NUM_THREADS 8

typedef struct
{
    int thrd_id;
    queue_t *queue;
} thrd_data_t;

// int func(void *arg)
// {
//     int id = (int)arg;
//     uint64_t hash = 0;
//     printf("%d starting\n", id);
//     ph_dct_imagehash("sample.jpg", &hash);
//     printf("%d %.16lx\n", id, hash);
// }

// int producer(void *arg)
// {
//     queue_t *q = (queue_t *)arg;
//     for (int i = 1; i < 50; i++)
//     {
//         printf("producer %d\n", i);
//         queue_put(q, i);
//     }
//     thrd_exit(0);
// }

int consumer(void *arg)
{
    thrd_data_t *d = arg;
    struct timespec delay;

    while (1)
    {
        void *i;
        if (!queue_get(d->queue, &i))
            thrd_exit(0);

        delay.tv_nsec = rand() % 100 * 10000000;
        printf("consumer %d %d %d\n", d->thrd_id, delay.tv_nsec, i);
        thrd_sleep(&delay, NULL);
    }
}

int main(void)
{
    thrd_t threads[NUM_THREADS];
    queue_t *q = queue_init();

    for (int i = 0; i < 128; i++)
        queue_put(q, i);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thrd_data_t *d = malloc(sizeof(thrd_data_t));
        d->thrd_id = i;
        d->queue = q;
        thrd_create(&threads[i], consumer, d);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        thrd_join(threads[i], NULL);

    // thrd_t prodThr, consThr;

    // thrd_create(&prodThr, producer, q);
    // thrd_create(&consThr, consumer, q);

    // // TODO bad example, consumer starts too quickly and receives 0 (empty queue), so it exits
    // thrd_join(prodThr, NULL);
    // thrd_join(consThr, NULL);

    queue_destroy(q);

    // int n = 0;
    // uint8_t *mh_hash = ph_mh_imagehash("sample.jpg", &n, 2.0f, 1.0f);
    // for (int i = 0; i < n; i++)
    //     printf("%.2x", mh_hash[i]);
    // printf("\n");
    // free(mh_hash);

    return 0;
}
