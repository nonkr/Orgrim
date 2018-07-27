/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/18 20:15
 *
 */

#include <cstdio>
#include <cstring>
#include "../../color.h"

#define FOO_SIZE 4

void printHex(const char *data, int size)
{
    int i = 0;
    for (; i < size; i++)
    {
        printf("%02X ", data[i] & 0xFF);
    }
    printf("\n");
}

void local_variable()
{
    OGM_PRINT_BLUE("local_variable...\n");
    char foo[FOO_SIZE];

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

void local_dynamic_variable(int size)
{
    OGM_PRINT_BLUE("local_dynamic_variable...\n");
    char foo[size];

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

void local_variable_with_memset()
{
    OGM_PRINT_BLUE("local_variable_with_memset...\n");
    char foo[FOO_SIZE];
    memset(foo, 0, sizeof(foo));

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

void local_static()
{
    OGM_PRINT_BLUE("local_static...\n");
    static char foo[FOO_SIZE];

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

void local_static_with_memset()
{
    OGM_PRINT_BLUE("local_static_with_memset...\n");
    static char foo[FOO_SIZE];
    memset(foo, 0, sizeof(foo));

    printHex(foo, sizeof(foo));

    foo[0] = 0x01;

    printHex(foo, sizeof(foo));
}

int main()
{
    local_variable();
    local_variable();

    local_dynamic_variable(1);
    local_dynamic_variable(2);

    local_variable_with_memset();
    local_variable_with_memset();

    local_static();
    local_static();

    local_static_with_memset();
    local_static_with_memset();

    return 0;
}
