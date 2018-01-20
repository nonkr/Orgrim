/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
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

int main()
{
    auto start = system_clock::now();

    // 一段计算
    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }

    auto end      = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    printf("%f\n", double(duration.count()) * microseconds::period::num / microseconds::period::den);
    return 0;
}