/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/6 14:30
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../../utils/print_utils.h"

typedef struct __attribute__((packed))
{
    int   a;
    short b;
} Foo;

typedef struct __attribute__((packed))
{
    Foo       foo[3];
    long long llLong;
    char      c;
} Bar;

int main()
{
    Bar bar;

    memset(&bar, 0x00, sizeof(Bar));
    bar.foo[0].a = 0x1234;
    bar.foo[0].b = 0x02;
    bar.foo[1].a = 0x1234;
    bar.foo[1].b = 0x04;
    bar.foo[2].a = 0x1234;
    bar.foo[2].b = 0x06;
    bar.llLong   = 0x1234;
    bar.c        = 0x0C;

    print_as_hexstring((char *) &bar, sizeof(Bar));

    return EXIT_SUCCESS;
}
