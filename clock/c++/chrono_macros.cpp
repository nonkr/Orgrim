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

#define GCLOCK system_clock::time_point
#define TICK(S) GCLOCK S = system_clock::now();
#define TOCK(S, E, LABEL) GCLOCK E = system_clock::now(); \
                do { \
                    auto duration = duration_cast<microseconds>((E) - (S)); \
                    printf("Time duration of %s: %.3f sec.\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define GTICK(X) do { \
                    X = system_clock::now(); \
                 } while(false);
#define GTOCK TOCK

GCLOCK g_A;

void a()
{
    GTICK(g_A);
}

void b()
{
    GTOCK(g_A, g_B, "Global");
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

    TOCK(aa, bb, "First");

    a();
    // 一段计算
    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }
    b();

    TOCK(bb, cc, "Second");

    return 0;
}
