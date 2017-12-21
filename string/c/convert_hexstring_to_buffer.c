/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/21 13:44
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int hexstring_to_bytearray(char *p_hexstring, char **pp_out, int *p_i_out_len)
{
    int    i;
    size_t i_str_len;
    char   *p_out    = NULL;
    int    i_out_len = 0;
    int    i_offset  = 0;

    if (p_hexstring == NULL)
        return -1;

    i_str_len = strlen(p_hexstring);

    // 先初步对传入的字符串做一遍检查
    if (i_str_len == 2)
        i_out_len = 1;
    else if ((i_str_len - 2) % 3 == 0)
        i_out_len = (i_str_len - 2) / 3 + 1;
    else
    {
        fprintf(stderr, "malformed input hex string\n");
        goto error;
    }

    p_out = (char *) malloc((size_t) i_out_len);
    if (p_out == NULL)
        return -1;

    for (i = 0; i < i_str_len;)
    {
        if (isxdigit(p_hexstring[i]) && isxdigit(p_hexstring[i + 1]))
        {
            sscanf(p_hexstring + i, "%2x", (unsigned int *) &(p_out[i_offset++]));

            if (i + 2 == i_str_len)
                break;

            if (p_hexstring[i + 2] != ' ')
            {
                fprintf(stderr, "malformed input hex string\n");
                goto error;
            }

            i += 3;
        }
        else
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }
    }

    *pp_out      = p_out;
    *p_i_out_len = i_out_len;

    return 0;
error:
    if (p_out)
        free(p_out);
    return -1;
}

void print_as_hexstring(const char *p_data, int i_data_len)
{
    int i;
    for (i = 0; i < i_data_len; i++)
    {
        printf("%02X", p_data[i] & 0xFF);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s \"00 01 02\"\n", argv[0]);
        return -1;
    }

    char *p_buffer    = NULL;
    int  i_buffer_len = 0;

    if (hexstring_to_bytearray(argv[1], &p_buffer, &i_buffer_len))
    {
        fprintf(stderr, "convert failed.\n[%s]\n", argv[1]);
        return -2;
    }

    print_as_hexstring(p_buffer, i_buffer_len);

    if (p_buffer)
    {
        free(p_buffer);
        p_buffer = NULL;
    }

    return 0;
}