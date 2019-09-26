/*
 * Copyright (c) 2019, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/9/26 18:31
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

void test1()
{
    char       *startTimeString = "2019-09-26 14:57:00";
    char       *endTimeString   = "2019-09-26 15:57:01";
    const char *pFormat         = "%Y-%m-%d %H:%M:%S"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    struct tm  tmStart          = {0};
    struct tm  tmEnd            = {0};

    setenv("TZ", "GMT", 1);
    tzset();

    strptime(startTimeString, pFormat, &tmStart);
    strptime(endTimeString, pFormat, &tmEnd);

    time_t startEpoch = mktime(&tmStart);
    time_t endEpoch   = mktime(&tmEnd);
    time_t diff       = endEpoch - startEpoch;
    printf("startEpoch:%ld\n", startEpoch);
    printf("tmEnd:     %ld\n", endEpoch);
    printf("diff:      %f\n", difftime(endEpoch, startEpoch));

    char      tmBuffer[64] = {0};
    struct tm *ptr         = localtime(&diff);
    strftime(tmBuffer, 64, "%H:%M:%S", ptr);
    printf("%s\n", tmBuffer);
}

int main(void)
{
    test1();
    return 0;
}
