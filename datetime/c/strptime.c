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
    char       *szBuffer    = __DATE__;
    const char *pFormat     = "%b %d %Y"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    char       tmBuffer[64] = {0};
    struct tm  tmTemp;

    strptime(szBuffer, pFormat, &tmTemp);
    strftime(tmBuffer, 64, "%y%m%d", &tmTemp);
    printf("%s\n", tmBuffer);

    return 0;
}
