/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/2/2 14:03
 *
 */

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

#include "../../../color.h"

#define MAX 10

sem_t sem1, sem2;

void *thread_func_1(void *arg)
{
    int i;
    for (i = 0; i < MAX; i++)
    {
        sem_wait(&sem1);
        OGM_PRINT_GREEN("number in thread1 is %d\n", i);
        sem_post(&sem2);
    }
    pthread_exit((void *) "thread1 exit\n");

}

void *thread_func_2(void *arg)
{
    int i;
    for (i = 0; i < MAX; i++)
    {
        sem_wait(&sem2);
        OGM_PRINT_YELLOW("number in thread2 is %d\n", i);
        sem_post(&sem1);
    }
    pthread_exit((void *) "thread2 exit\n");
}

int main(void)
{
    void *ret;
    sem_init(&sem1, 0, 5);
    sem_init(&sem2, 0, 5);
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread_func_1, NULL);
    pthread_create(&tid2, NULL, thread_func_2, NULL);

    pthread_join(tid1, &ret);
    pthread_join(tid2, &ret);
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;

}