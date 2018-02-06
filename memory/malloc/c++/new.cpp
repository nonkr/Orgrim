/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/26 19:12
 *
 */

#include <cstdio>

int main()
{
    auto *p = new char[10]();

    printf("p1:[%p]\n", (void *) p);

    printf("%02X\n", p[0]);
    printf("%02X\n", p[1]);

    delete[] p;
    printf("p2:[%p]\n", (void *) p);

    p = nullptr;
    printf("p3:[%p]\n", (void *) p);

    return 0;
}