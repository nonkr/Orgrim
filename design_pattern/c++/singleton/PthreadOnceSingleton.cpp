/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/30 15:29
 *
 *
 *  NOTE: 本例亦适用于C++11之前的多线程环境。
 *  通过unix平台下的pthread_once函数保证Init只被调用一次。
 */

#include <pthread.h>
#include <cstdio>
#include <cstdlib>

class PthreadOnceSingleton
{
public:
    static PthreadOnceSingleton &getInstance()
    {
        printf("getInstance\n");
        pthread_once(&_once_control, Init);
        return *_value;
    }

private:
    static pthread_once_t       _once_control;
    static PthreadOnceSingleton *_value;

    PthreadOnceSingleton()
    {
        printf("c'tor\n");
    };

    ~PthreadOnceSingleton()
    {
        printf("d'tor\n");
    };

    PthreadOnceSingleton(const PthreadOnceSingleton &);

    PthreadOnceSingleton &operator=(const PthreadOnceSingleton &);

    static void Init()
    {
        printf("Init\n");
        _value = new PthreadOnceSingleton();
        atexit(DeInit);
    }

    static void DeInit()
    {
        printf("DeInit\n");
        delete _value;
    }
};

pthread_once_t       PthreadOnceSingleton::_once_control = PTHREAD_ONCE_INIT;
PthreadOnceSingleton *PthreadOnceSingleton::_value       = nullptr;

int main()
{
    printf("main\n");
    PthreadOnceSingleton &s1 = PthreadOnceSingleton::getInstance();
    PthreadOnceSingleton &s2 = PthreadOnceSingleton::getInstance();

    printf("s1:%p\n", &s1);
    printf("s2:%p\n", &s2);

    return 0;
}
