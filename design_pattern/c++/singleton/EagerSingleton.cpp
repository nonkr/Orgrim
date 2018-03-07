/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/7 11:27
 *
 */

#include <cstdio>

class EagerSingleton
{
protected:
    EagerSingleton() {}

private:
    static EagerSingleton *p;
public:
    static EagerSingleton *getInstance();
};

EagerSingleton *EagerSingleton::p = new EagerSingleton;

EagerSingleton *EagerSingleton::getInstance()
{
    return p;
}

int main()
{
    auto s1 = EagerSingleton::getInstance();
    auto s2 = EagerSingleton::getInstance();

    printf("s1:%p\n", s1);
    printf("s2:%p\n", s2);

    return 0;
}
