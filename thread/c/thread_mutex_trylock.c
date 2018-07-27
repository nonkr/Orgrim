/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/27 17:17
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t G_MUTEX;
int             G_INT;

void *thread_routine1(void *args)
{
    printf("Routine 1\n");
    pthread_mutex_lock(&G_MUTEX);

    sleep(2);

    printf("G_INT:%d in routine1\n", G_INT);

    pthread_mutex_unlock(&G_MUTEX);

    pthread_exit(NULL);
}

void *thread_routine2(void *args)
{
    printf("Routine 2\n");
    if (pthread_mutex_trylock(&G_MUTEX) == 0)
    {
        printf("routine 2 trylock success\n");

        G_INT = 2;

        sleep(1);

        pthread_mutex_unlock(&G_MUTEX);
    }
    else
    {
        printf("routine 2 trylock failed\n");
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t thread1 = 0;
    pthread_t thread2 = 0;

    G_INT = 1;

    pthread_mutex_init(&G_MUTEX, NULL);

    if (pthread_create(&thread1, NULL, thread_routine1, NULL))
    {
        printf("failed to create thread\n");
        exit(-1);
    }

    sleep(1);

    if (pthread_create(&thread2, NULL, thread_routine2, NULL))
    {
        printf("failed to create thread\n");
        exit(-1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&G_MUTEX);

    return 0;
}
