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

void test1()
{
    char       *szBuffer    = __DATE__ " " __TIME__;
    const char *pFormat     = "%b %d %Y %H:%S:%M"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    char       tmBuffer[64] = {0};
    struct tm  tmTemp       = {0};

    strptime(szBuffer, pFormat, &tmTemp);
    strftime(tmBuffer, 64, "%y-%m-%d %H:%S:%M", &tmTemp);

    printf("test1:\n");
    printf("src:[%s]\n", szBuffer);
    printf("%s\n", tmBuffer);
}

void test2()
{
    char       *szBuffer    = "2019-09-26 14:57:00";
    const char *pFormat     = "%Y-%m-%d %H:%S:%M"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    char       tmBuffer[64] = {0};
    struct tm  tmTemp       = {0};

    strptime(szBuffer, pFormat, &tmTemp);
    strftime(tmBuffer, 64, "%Y-%m-%d %H:%S:%M", &tmTemp);

    printf("\ntest2:\n");
    printf("src:[%s]\n", szBuffer);
    printf("%s\n", tmBuffer);

    time_t timeSinceEpoch = mktime(&tmTemp);
    printf("timeSinceEpoch:%ld\n", timeSinceEpoch);
}

void test3()
{
    char       *szBuffer    = "Wed Jan 13 17:20:07 CST 2021";
    const char *pFormat     = "%a %b %d %H:%S:%M CST %Y"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    char       tmBuffer[64] = {0};
    struct tm  tmTemp       = {0};

    strptime(szBuffer, pFormat, &tmTemp);
    strftime(tmBuffer, 64, "%Y-%m-%d %H:%S:%M", &tmTemp);

    printf("\ntest3:\n");
    printf("src:[%s]\n", szBuffer);
    printf("%s\n", tmBuffer);
}

int main(void)
{
    test1();

    test2();

    test3();

    return 0;
}
