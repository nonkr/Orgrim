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

class LazySingleton1
{
protected:
    LazySingleton1()
    {
        pthread_mutex_init(&mutex, nullptr);
    }

private:
    static LazySingleton1 *p;
public:
    static pthread_mutex_t mutex;

    static LazySingleton1 *getInstance();
};

pthread_mutex_t LazySingleton1::mutex;
LazySingleton1  *LazySingleton1::p = nullptr;

LazySingleton1 *LazySingleton1::getInstance()
{
    if (p == nullptr)
    {
        pthread_mutex_lock(&mutex);
        if (p == nullptr)
            p = new LazySingleton1();
        pthread_mutex_unlock(&mutex);
    }
    return p;
}

int main()
{
    auto s1 = LazySingleton1::getInstance();
    auto s2 = LazySingleton1::getInstance();

    printf("s1:%p\n", s1);
    printf("s2:%p\n", s2);

    return 0;
}
