/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/7 11:31
 *
 *
 * NOTE: 本例中的方法适用于单线程和C++11及以后版本的多线程环境，但不适用于C++11之前的多线程环境
 *
 */

#include <cstdio>

class MeyersSingleton
{
public:
    static MeyersSingleton &getInstance()
    {
        printf("getInstance\n");
        static MeyersSingleton instance;
        return instance;
    }

private:
    MeyersSingleton()
    {
        printf("c'tor\n");
    };

    ~MeyersSingleton()
    {
        printf("d'tor\n");
    };

    MeyersSingleton(const MeyersSingleton &);

    MeyersSingleton &operator=(const MeyersSingleton &);
};

int main()
{
    MeyersSingleton &s1 = MeyersSingleton::getInstance();
    MeyersSingleton &s2 = MeyersSingleton::getInstance();

    printf("s1:%p\n", &s1);
    printf("s2:%p\n", &s2);

    return 0;
}
