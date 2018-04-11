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

int main(void)
{
    time_t     lt           = time(NULL);
    struct tm  *ptr         = localtime(&lt);
    char       szBuffer[64] = {0};
    const char *pFormat     = "The time now is %Y-%m-%d %H:%M:%S";\

    strftime(szBuffer, 64, pFormat, ptr);
    printf("%s\n", szBuffer);

    return 0;
}
