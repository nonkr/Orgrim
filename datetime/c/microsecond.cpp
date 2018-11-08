/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/11/8 22:26
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main()
{
    struct timeval tv;
    struct tm      *p_tm         = NULL;
    char           arr_buffer[64] = {0};

    gettimeofday(&tv, NULL);

    p_tm = localtime(&(tv.tv_sec));

    snprintf(arr_buffer, 64, "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
             p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
             p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec, tv.tv_usec);

    printf("time:[%s]\n", arr_buffer);

    return 0;
}
