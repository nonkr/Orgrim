/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/5/19 13:41
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *PrintHello(void *args)
{
    printf("Hello World! It's me!\n");

    sleep(1);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t threads = 0;

    if (pthread_create(&threads, NULL, PrintHello, NULL))
    {
        printf("failed to create thread\n");
        exit(-1);
    }

    pthread_join(threads, NULL);

    /* Last thing that main() should do */
    return 0;
}
