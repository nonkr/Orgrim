/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/2 13:46
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_barrier_t barrier;

void *initor(void *param)
{
    int *i = (int *) param;
    printf(">>>>>[thread%d] ready to work!\n", *i);

    /* 模拟子线程初始化的工作。这期间主线程会处于等待状态 */
    sleep(3);
    printf(">>>>>[thread%d] init completed!\n", *i);
    pthread_barrier_wait(&barrier);

    /* 模拟子线程的工作。这期间主线程和子线程各忙各的。 */
    sleep(3);
    printf(">>>>>[thread%d] work completed!\n", *i);
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_barrier_init(&barrier, NULL, 3);
    printf("=======[main] pthread_barrier_init done ===========\n");

    pthread_t pid1;
    pthread_t pid2;

    int i = 1;
    pthread_create(&pid1, NULL, &initor, &i);

    int j = 2;
    pthread_create(&pid2, NULL, &initor, &j);

    /* 等待子线程完成初始化工作后，才能继续往下执行。 */
    printf("=======[main] waiting for pthread's good news! ===========\n");
    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);
    printf("=======[main] yeah! pthread well done! ========\n");

    /* 模拟主线程的工作 */
    sleep(2);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
    printf("======= good luck and good bye!\n");

    return 0;
}