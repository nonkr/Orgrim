/*
 * Copyright (c) 2019, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/11/10 16:16
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctime>
#include <chrono>
#include <iostream>
#include <cstring>

using namespace std;
using namespace chrono;

void test1()
{
    auto tp = system_clock::now();

    auto ttime_t = system_clock::to_time_t(tp);

    std::tm *ttm = localtime(&ttime_t);

    char date_time_format[] = "%Y-%m-%d %H:%M:%S";

    char time_str[20];

    std::strftime(time_str, sizeof(time_str), date_time_format, ttm);

    printf("test1:[%s]\n", time_str);
}

void test2()
{
    std::time_t result = std::time(nullptr) + (8 * 60 * 60);

    std::tm *ttm = gmtime(&result);

    char date_time_format[] = "%Y-%m-%d %H:%M:%S";

    char time_str[20];

    std::strftime(time_str, sizeof(time_str), date_time_format, ttm);

    printf("test2:[%s]\n", time_str);
}

int main()
{
    test1();
    test2();
    return 0;
}
