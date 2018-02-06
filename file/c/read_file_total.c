/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/22 14:14
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "../../color.h"

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    char *file = argv[1];
    FILE *fp   = NULL;
    long l_total_size;
    int  i_read_len;
    long l_left_len;
    char *buff = NULL;

    if ((fp = fopen(file, "rb")) == NULL)
    {
        fprintf(stderr, "open %s failed\n", file);
        return 1;
    }

    if (fseek(fp, 0, SEEK_END))
    {
        perror("seek error");
        fclose(fp);
        return 1;
    }
    l_total_size = ftell(fp);
    OGM_PRINT_GREEN("File total size:[%ld]\n", l_total_size);

    if ((buff = malloc(sizeof(char) * l_total_size)) == NULL)
    {
        fprintf(stderr, "malloc error\n");
        return 1;
    }
    memset(buff, 0x00, sizeof(char) * l_total_size);

    if (fseek(fp, 0, SEEK_SET))
    {
        perror("seek error");
        fclose(fp);
        return 1;
    }
    while (!feof(fp))
    {
        i_read_len = fread(buff, sizeof(char), l_total_size, fp);
        if (i_read_len > 0)
        {
            l_left_len = l_total_size - ftell(fp);
            printf("i_read_len:[%d] l_left_len:[%ld]\n", i_read_len, l_left_len);
        }
    }

    fclose(fp);

    return 0;
}