/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/11 18:43
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    time_t     lt            = time(NULL);
    struct tm  *ptr          = localtime(&lt);
    char       szBuffer[64]  = {0};
    char       szBuffer2[64] = {0};
    const char *pFormat      = "%Y-%m-%d %H:%M:%S";
    strftime(szBuffer, 64, pFormat, ptr);

    sleep(2);

    time_t    lt2   = time(NULL);
    struct tm *ptr2 = localtime(&lt2);
    strftime(szBuffer2, 64, pFormat, ptr2);
    printf("%s -> %s\n", szBuffer, szBuffer2);

    return 0;
}
