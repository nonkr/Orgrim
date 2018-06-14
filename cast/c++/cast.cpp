/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/6/14 14:56
 *
 */

#include <cstring>
#include <cstdio>
#include <netinet/in.h>
#include "../../utils/print_utils.h"

int main()
{
    float f = 12345.67;
    char a[6] = {0};
    char b[12] = {0};
    int i = 0;
    b[++i] = 0x01;
    short s = 0x3039;

    union v
    {
        float f;
        char  c[4];
    };

    v val = {0};
    val.f = f;

    printf("f: ");
    print_as_hexstring((char *) &f, sizeof(f));

    printf("val.c: ");
    print_as_hexstring(val.c, sizeof(v));

    memcpy(a + 2, val.c, sizeof(v));
    printf("a: ");
    print_as_hexstring(a, sizeof(a));

    memcpy(b + (++i), &f, sizeof(f));
    printf("b: ");
    print_as_hexstring(b, sizeof(b));

    printf("0x%02X 0x%02X\n", s >> 8, s & 0xFF);

    return 0;
}
