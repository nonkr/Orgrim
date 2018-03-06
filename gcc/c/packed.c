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
    char      no;
    Foo       *foo;
    long long llLong;
    char      c;
} Bar;

int main()
{
    int i;
    Bar bar;

    memset(&bar, 0x00, sizeof(Bar));

    bar.no  = 3;
    bar.foo = malloc(sizeof(Foo) * bar.no);

    for (i = 0; i < bar.no; i++)
    {
        bar.foo[i].a = 0x1234;
        bar.foo[i].b = (short) (i + 1);
    }

    bar.llLong = 0x1234;
    bar.c      = 0x0C;

    print_as_hexstring((char *) bar.foo, sizeof(Foo) * bar.no);
    print_as_hexstring((char *) &bar, sizeof(Bar));

    free(bar.foo);

    return EXIT_SUCCESS;
}
