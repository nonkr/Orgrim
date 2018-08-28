/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/27 13:52
 *
 */

#include <memory.h>
#include <stdio.h>

int main()
{
    unsigned char str[4] = {0xAA, 0x01, 0x02, 0x03};

    memmove(str + 1, str, 3);

    int i;
    for (i = 0; i < 4; i++)
    {
        printf("0x%02X ", str[i]);
    }
    printf("\n");

    return 0;
}
