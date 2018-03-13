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

#define TOCK(X) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    printf("time duration %s: %.3f sec.\n", (#X), double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(0);

int main()
{
    TICK(aa);

    // 一段计算
    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }

    TOCK(aa);

    return 0;
}
