/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/12/15 12:34
 *
 */

#include <iostream>
#include <chrono>
#include <cstdio>
#include <unistd.h>

using namespace std;
using namespace chrono;

struct RuntimeInfo
{
    system_clock::time_point t;
};

RuntimeInfo g_stRuntimeInfo;

int main()
{
    system_clock::time_point t;

    printf("init: %ld\n", t.time_since_epoch().count());

    t = system_clock::now();

    printf("now: %ld\n", t.time_since_epoch().count());

//    g_stRuntimeInfo.t = system_clock::from_time_t(0);
    printf("g_stRuntimeInfo.t: %ld\n", g_stRuntimeInfo.t.time_since_epoch().count());

    return 0;
}
