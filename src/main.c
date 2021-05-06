#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pHash_c.h>

#include "queue.h"

#define NUM_THREADS 8

typedef struct
{
    int thread_id;
    queue_t *queue;
} thread_data_t;

// TODO flag is needed when the queue is not fully populated before starting threads

// int func(void *arg)
// {
//     int id = (int)arg;
//     uint64_t hash = 0;
//     printf("%d starting\n", id);
//     ph_dct_imagehash("sample.jpg", &hash);
//     printf("%d %.16lx\n", id, hash);
// }

void *worker_func(void *arg)
{
    thread_data_t *d = arg;
    //struct timespec delay, watch1, watch2;

    while (1)
    {
        void *i;
        if (!queue_get(d->queue, &i))
            break;

        printf("thread %d start\n", d->thread_id);
        //clock_gettime(CLOCK_REALTIME, &watch1);

        // uint64_t hash = 0;
        // bool r = ph_c_dct_imagehash("sample.jpg", &hash);

        uint8_t *hash;
        bool r = ph_c_mh_imagehash("sample.jpg", 2.f, 1.f, &hash);

        //clock_gettime(CLOCK_REALTIME, &watch2);
        printf("thread %d end:%d\n", d->thread_id, r);

        //printf("thread %d %.16lx %ld.%ld %ld.%ld\n", d->thread_id, hash, watch1.tv_sec, watch1.tv_nsec, watch2.tv_sec, watch2.tv_nsec);

        // delay.tv_nsec = rand() % 100 * 10000000;
        // printf("thread %d %d %d\n", d->thread_id, delay.tv_nsec, i);
        //thrd_sleep(&delay, NULL);
    }

    free(d);
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    queue_t *q = queue_init();

    printf("Hello there\n");

    for (int i = 0; i < 32; i++)
        queue_put(q, i);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_data_t *d = malloc(sizeof(thread_data_t));
        d->thread_id = i;
        d->queue = q;
        pthread_create(&threads[i], NULL, worker_func, d);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    queue_destroy(q);

    // int n = 0;
    // uint8_t *mh_hash = ph_mh_imagehash("sample.jpg", &n, 2.0f, 1.0f);
    // for (int i = 0; i < n; i++)
    //     printf("%.2x", mh_hash[i]);
    // printf("\n");
    // free(mh_hash);

    printf("ffffffff\n");

    return 0;
}
