/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/30 11:39
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

pthread_once_t once = PTHREAD_ONCE_INIT;

void once_run(void)
{
    printf("once_run in thread %u\n", (unsigned int) pthread_self());
}

void *child1(void *arg)
{
    pthread_t tid = pthread_self();
    printf("thread %u enter\n", (unsigned int) tid);
    pthread_once(&once, once_run);
    printf("thread %u return\n", (unsigned int) tid);
    pthread_exit(NULL);
}


void *child2(void *arg)
{
    pthread_t tid = pthread_self();
    printf("thread %u enter\n", (unsigned int) tid);
    pthread_once(&once, once_run);
    printf("thread %u return\n", (unsigned int) tid);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid1, tid2;
    printf("hello\n");
    pthread_create(&tid1, NULL, child1, NULL);
    pthread_create(&tid2, NULL, child2, NULL);
    sleep(3);
    printf("main thread exit\n");
    return 0;
}
