/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/7 11:31
 *
 */

#include <cstdio>
#include <pthread.h>

class LazySingleton2
{
protected:
    LazySingleton2()
    {
        pthread_mutex_init(&mutex, nullptr);
    }

public:
    static pthread_mutex_t mutex;

    static LazySingleton2 *getInstance();

    int a;
};

pthread_mutex_t LazySingleton2::mutex;

LazySingleton2 *LazySingleton2::getInstance()
{
    pthread_mutex_lock(&mutex);
    static LazySingleton2 obj;
    pthread_mutex_unlock(&mutex);
    return &obj;
}

int main()
{
    auto s1 = LazySingleton2::getInstance();
    auto s2 = LazySingleton2::getInstance();

    printf("s1:%p\n", s1);
    printf("s2:%p\n", s2);

    return 0;
}
