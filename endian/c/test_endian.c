/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/28 15:57
 *
 */

#include <stdio.h>
#include <netinet/in.h>
#include "../../utils/print_utils.h"

static float float_swap(float value)
{
    union v
    {
        float        f;
        unsigned int i;
    };

    union v val;

    val.f = value;
    val.i = htonl(val.i);

    return val.f;
};

int main()
{
    union w
    {
        int  a; //4 bytes
        char b; //1 byte
    } c;

    c.a = 1;

    if (c.b == 1)
    {
        printf("Little endian\n");
    }
    else
    {
        printf("Big endian\n");
    }

    float f = 1.2345;
    print_as_hexstring((char *) &f, sizeof(f));

    float f2 = float_swap(f);
    print_as_hexstring((char *) &f2, sizeof(f2));

    float f3 = float_swap(f2);
    print_as_hexstring((char *) &f3, sizeof(f3));

    return 1;
}
