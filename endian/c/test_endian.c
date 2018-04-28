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

#include "stdio.h"

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

    return 1;
}
