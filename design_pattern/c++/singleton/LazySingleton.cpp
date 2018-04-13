/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/7 11:30
 *
 */

#include <cstdio>
#include <pthread.h>

class LazySingleton
{
protected:
    LazySingleton()
    {
        pthread_mutex_init(&mutex, nullptr);
    }

private:
    static LazySingleton *p;
public:
    static pthread_mutex_t mutex;

    static LazySingleton *getInstance();
};

pthread_mutex_t LazySingleton::mutex;
LazySingleton  *LazySingleton::p = nullptr;

LazySingleton *LazySingleton::getInstance()
{
    if (p == nullptr)
    {
        pthread_mutex_lock(&mutex);
        if (p == nullptr)
            p = new LazySingleton();
        pthread_mutex_unlock(&mutex);
    }
    return p;
}

int main()
{
    auto s1 = LazySingleton::getInstance();
    auto s2 = LazySingleton::getInstance();

    printf("s1:%p\n", s1);
    printf("s2:%p\n", s2);

    return 0;
}
