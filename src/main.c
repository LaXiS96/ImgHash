#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pHash_c.h>

#include "queue.h"

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

// Memory usage depends only on CImg since it loads the full image into memory
// 24 threads average around 700MB of used RAM
#define NUM_THREADS 24

#define FSIG_BYTES 8

const uint8_t jpeg_fsig[] = {0xFF, 0xD8, 0xFF};
const uint8_t png_fsig[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
const uint8_t tiff_le_fsig[] = {0x49, 0x49, 0x2A, 0x00};
const uint8_t tiff_be_fsig[] = {0x4D, 0x4D, 0x00, 0x2A};

typedef struct
{
    int thread_id;
    queue_t *queue;
} thread_data_t;

// TODO flag is needed when the queue is not fully populated before starting threads

void *worker_func(void *arg)
{
    thread_data_t *d = arg;
    //struct timespec delay, watch1, watch2;

    while (1)
    {
        char *fpath;
        if (!queue_get(d->queue, (void *)&fpath))
            break;

        //clock_gettime(CLOCK_REALTIME, &watch1);

        uint64_t hash = 0;
        bool r = ph_c_dct_imagehash(fpath, &hash);
        if (r)
            printf("thread %d fpath %s hash %.16llx\n", d->thread_id, fpath, hash);
        else
            printf("thread %d fpath %s ERROR\n", d->thread_id, fpath);

        // uint8_t *hash;
        // bool r = ph_c_mh_imagehash("sample.jpg", 2.f, 1.f, &hash);

        //clock_gettime(CLOCK_REALTIME, &watch2);
        // printf("thread %d end:%d\n", d->thread_id, r);

        //printf("thread %d %.16lx %ld.%ld %ld.%ld\n", d->thread_id, hash, watch1.tv_sec, watch1.tv_nsec, watch2.tv_sec, watch2.tv_nsec);

        // delay.tv_nsec = rand() % 100 * 10000000;
        // printf("thread %d %d %d\n", d->thread_id, delay.tv_nsec, i);
        //thrd_sleep(&delay, NULL);

        free(fpath);
    }

    free(d);
}

int main(void)
{
    const char *path = "CHANGEME";

    DIR *dir;
    queue_t *q = queue_init();
    int count = 0;
    pthread_t threads[NUM_THREADS];

    if ((dir = opendir(path)) == NULL)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    struct dirent *e;
    while ((e = readdir(dir)) != NULL)
    {
        // Skip dotfiles
        if (e->d_name[0] == '.')
            continue;

        // Build file path
        char *fpath = malloc((PATH_MAX + 1) * sizeof(char));
        strncpy(fpath, path, PATH_MAX);
        strncat(fpath, PATH_SEP, PATH_MAX);
        strncat(fpath, e->d_name, PATH_MAX);

        // Check that it's a regular file
        struct stat s;
        if (stat(fpath, &s) == -1)
        {
            perror("stat");
            free(fpath);
            continue;
        }

        if (!S_ISREG(s.st_mode))
        {
            free(fpath);
            continue;
        }

        // Open it for binary reading
        FILE *f = fopen(fpath, "rb");
        if (f == NULL)
        {
            perror("fopen");
            free(fpath);
            continue;
        }

        // Check that it matches a known file signature
        uint8_t data[FSIG_BYTES] = {0};
        fread(&data, sizeof(uint8_t), FSIG_BYTES, f);
        if (memcmp(data, jpeg_fsig, sizeof(jpeg_fsig)) != 0 &&
            memcmp(data, png_fsig, sizeof(png_fsig)) != 0 &&
            memcmp(data, tiff_le_fsig, sizeof(tiff_le_fsig)) != 0 &&
            memcmp(data, tiff_be_fsig, sizeof(tiff_be_fsig)) != 0)
        {
            free(fpath);
            fclose(f);
            continue;
        }

        queue_put(q, fpath);
        count++;
        fclose(f);
    }
    closedir(dir);

    printf("Queue filled (%d items)\n", count);

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

    return EXIT_SUCCESS;
}
