/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/3 23:08
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "threadpool.h"

void *work_routine(void *arg)
{
    int arg_v = *(int *) arg;
    printf("Working on %d. Sleeping for %d seconds.\n", arg_v, 3);
    sleep(3);
    printf("Work done (%d).\n", arg_v);
    return NULL;
}

int main()
{
    thread_pool *pool = pool_init(4);
    sleep(1);

    printf("Testing threadpool of %d threads.\n", pool_get_max_threads(pool));

    for (int i = 1; i <= 8; i++)
    {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pool_add_task(pool, work_routine, (void *) arg);
    }
    printf("All scheduled!\n");

    sleep(1);

    pool_wait(pool);
    pool_destroy(pool);

    printf("Done.\n");
}
