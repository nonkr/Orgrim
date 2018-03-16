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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
                      ((byte) & 0x80 ? '1' : '0'), \
                      ((byte) & 0x40 ? '1' : '0'), \
                      ((byte) & 0x20 ? '1' : '0'), \
                      ((byte) & 0x10 ? '1' : '0'), \
                      ((byte) & 0x08 ? '1' : '0'), \
                      ((byte) & 0x04 ? '1' : '0'), \
                      ((byte) & 0x02 ? '1' : '0'), \
                      ((byte) & 0x01 ? '1' : '0')

#define SHORT_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY_PRETTY_PATTERN BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY(s) BYTE_TO_BINARY((s) >> 8), BYTE_TO_BINARY(s)

#define INT_TO_BINARY_PATTERN SHORT_TO_BINARY_PATTERN""SHORT_TO_BINARY_PATTERN
#define INT_TO_BINARY_PRETTY_PATTERN SHORT_TO_BINARY_PRETTY_PATTERN" "SHORT_TO_BINARY_PRETTY_PATTERN
#define INT_TO_BINARY(i) BYTE_TO_BINARY((i) >> 24), \
                         BYTE_TO_BINARY((i) >> 16), \
                         BYTE_TO_BINARY((i) >> 8), \
                         BYTE_TO_BINARY(i)

#define LONGLONG_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN""INT_TO_BINARY_PATTERN
#define LONGLONG_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN" "INT_TO_BINARY_PRETTY_PATTERN
#define LONGLONG_TO_BINARY(ll) BYTE_TO_BINARY((ll) >> 56), \
                               BYTE_TO_BINARY((ll) >> 48), \
                               BYTE_TO_BINARY((ll) >> 40), \
                               BYTE_TO_BINARY((ll) >> 32), \
                               BYTE_TO_BINARY((ll) >> 24), \
                               BYTE_TO_BINARY((ll) >> 16), \
                               BYTE_TO_BINARY((ll) >> 8), \
                               BYTE_TO_BINARY(ll)

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
