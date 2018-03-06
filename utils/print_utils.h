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

void print_as_hexstring(const char *p_data, int i_data_len)
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
