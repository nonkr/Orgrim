/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/2/6 16:45
 *
 */

#include <cstdio>
#include <cstring>
#include "example.h"

extern "C" int FuncInC(int (*)(void *, int, int), void *pIn);

class D
{
private:
    int Fun01(int a, int b)
    {
        printf("C::Fun01: a=0x%02X, b=0x%02X\n", a, b);
        return 0;
    }

    static int __Fun01(void *context, int a, int b);

public:
    int Fun02(int a)
    {
        static Param p;
        memset(&p, 0x00, sizeof(Param));
        p.a = 0xCCBB;
        FuncInC(__Fun01, &p);
        return 0;
    }
};

int D::__Fun01(void *context, int a, int b)
{
    return ((D *) context)->Fun01(a, b);
}

int main()
{
    D c;
    return c.Fun02(0);
}