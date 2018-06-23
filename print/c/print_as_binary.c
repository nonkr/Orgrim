/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/16 12:37
 *
 */

#include <stdio.h>
#include "../../utils/print_utils.h"

int main(int argc, char **argv)
{
    int          b  = 0B10110110;
    char         c  = 0x44;
    unsigned int ui = 0x4c800D14;
    long long    ll = 0x61710A784c800D14;

    printf("b: [" BYTE_TO_BINARY_PATTERN "]\n", BYTE_TO_BINARY(b));
    printf("c: [" BYTE_TO_BINARY_PATTERN "]\n", BYTE_TO_BINARY(c));
    printf("ui:[" SHORT_TO_BINARY_PATTERN "]\n", SHORT_TO_BINARY(ui));
    printf("ui:[" SHORT_TO_BINARY_PRETTY_PATTERN "]\n", SHORT_TO_BINARY(ui));
    printf("ui:[" INT_TO_BINARY_PATTERN "]\n", INT_TO_BINARY(ui));
    printf("ui:[" INT_TO_BINARY_PRETTY_PATTERN "]\n", INT_TO_BINARY(ui));
    printf("ll:[" LONGLONG_TO_BINARY_PATTERN "]\n", LONGLONG_TO_BINARY(ll));
    printf("ll:[" LONGLONG_TO_BINARY_PRETTY_PATTERN "]\n", LONGLONG_TO_BINARY(ll));

    return 0;
}
