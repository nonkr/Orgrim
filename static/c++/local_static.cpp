/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/18 20:15
 *
 */

#include <cstdio>
#include <cstring>

void printHex(const char *data, int size)
{
    int i = 0;
    for (; i < size; i++)
    {
        printf("%02X ", data[i] & 0xFF);
    }
    printf("\n");
}

void local_static()
{
    static char foo[8];

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

void local_static_with_memset()
{
    static char foo[8];
    memset(foo, 0, sizeof(foo));

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

int main()
{
    local_static();
    local_static();

    local_static_with_memset();
    local_static_with_memset();

    return 0;
}