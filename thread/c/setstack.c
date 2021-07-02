#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

void *thread1(void *args)
{
    printf("thread1\n");

    pthread_attr_t attr;
    size_t         stacksize;
    /**
     * 这里有个坑，取pthread_attr_t时要用pthread_getattr_np，而不是pthread_attr_init，否则读出来的stacksize永远是默认值
     */
//    pthread_attr_init(&attr);
    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("thread1 stacksize:%zu\n", stacksize);
    pthread_attr_destroy(&attr);

    int  len2 = 1024 * 1024 * 3;
    char p2[len2];
    memset(p2, 0, len2);
    printf("Get Memory!!!\n");

    sleep(1);

    pthread_exit(NULL);
}

void *thread2(void *args)
{
    printf("thread2\n");

    pthread_attr_t attr;
    size_t         stacksize;
    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("thread2 stacksize:%zu\n", stacksize);
    pthread_attr_destroy(&attr);

    sleep(1);

    pthread_exit(NULL);
}

int main(void)
{
    size_t         stacksize = 1024 * 1024 * 4; // 4MB
    pthread_t      th1       = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stacksize);    // 设置线程栈大小
    if (pthread_create(&th1, &attr, thread1, NULL)) // 创建线程时传入attr
    {
        printf("failed to create thread1\n");
        exit(-1);
    }
    pthread_attr_destroy(&attr);

    sleep(1);

    pthread_t th2 = 0;
    if (pthread_create(&th2, NULL, thread2, NULL))
    {
        printf("failed to create thread2\n");
        exit(-1);
    }

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    return 0;
}
