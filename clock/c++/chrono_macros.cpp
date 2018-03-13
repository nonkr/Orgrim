/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/20 23:29
 *
 */

#include <iostream>
#include <cmath>
#include <chrono>
#include <cstdio>

using namespace std;
using namespace chrono;

#define TICK(X) auto X = system_clock::now();
#define GTICK(X) do { \
                    X = system_clock::now(); \
                 } while(false);
#define TOCK(X, LABEL) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    printf("Time duration of %s: %.3f sec.\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define GTOCK(X, LABEL) TOCK(X, LABEL)
#define GCLOCK system_clock::time_point

GCLOCK g_A;

void a()
{
    GTICK(g_A);
}

void b()
{
    GTOCK(g_A, "Global");
}

int main()
{
    TICK(aa);

    a();
    // 一段计算
    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }
    b();

    TOCK(aa, "Local");

    return 0;
}
