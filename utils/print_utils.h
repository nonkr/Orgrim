/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/6 14:34
 *
 */

#ifndef _PRINT_UTILS_H_
#define _PRINT_UTILS_H_

#include "../color.h"

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

#define SHORT_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN "" BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY_PRETTY_PATTERN BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY(s) BYTE_TO_BINARY((s) >> 8), BYTE_TO_BINARY(s)

#define INT_TO_BINARY_PATTERN SHORT_TO_BINARY_PATTERN "" SHORT_TO_BINARY_PATTERN
#define INT_TO_BINARY_PRETTY_PATTERN SHORT_TO_BINARY_PRETTY_PATTERN " " SHORT_TO_BINARY_PRETTY_PATTERN
#define INT_TO_BINARY(i) BYTE_TO_BINARY((i) >> 24), \
                         BYTE_TO_BINARY((i) >> 16), \
                         BYTE_TO_BINARY((i) >> 8), \
                         BYTE_TO_BINARY(i)

#define LONGLONG_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN "" INT_TO_BINARY_PATTERN
#define LONGLONG_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN " " INT_TO_BINARY_PRETTY_PATTERN
#define LONGLONG_TO_BINARY(ll) BYTE_TO_BINARY((ll) >> 56), \
                               BYTE_TO_BINARY((ll) >> 48), \
                               BYTE_TO_BINARY((ll) >> 40), \
                               BYTE_TO_BINARY((ll) >> 32), \
                               BYTE_TO_BINARY((ll) >> 24), \
                               BYTE_TO_BINARY((ll) >> 16), \
                               BYTE_TO_BINARY((ll) >> 8), \
                               BYTE_TO_BINARY(ll)

void print_bin_as_hexstring(const char *p_data, int i_data_len)
{
    int i;
    for (i = 0; i < i_data_len; i++)
    {
        if (i == i_data_len - 1)
        {
            OGM_PRINT_BLUE("%02X", p_data[i] & 0xFF);
        }
        else
        {
            OGM_PRINT_BLUE("%02X ", p_data[i] & 0xFF);
        }
    }
    OGM_PRINT_BLUE("\n");
}

#endif //_PRINT_UTILS_H_
