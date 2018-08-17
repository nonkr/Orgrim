/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/17 15:11
 *
 */

#include <iostream>
#include <thread>
#include <mutex>


volatile int         counter(0);    // non-atomic counter
std::recursive_mutex mtx;           // locks access to counter

void attempt_10k_increases2()
{
    for (int i = 0; i < 10; ++i)
    {
        mtx.lock();
        ++counter;
        mtx.unlock();
    }
}

void attempt_10k_increases1()
{
    for (int i = 0; i < 10000; ++i)
    {
        mtx.lock();
        ++counter;
        attempt_10k_increases2();
        mtx.unlock();
    }
}

int main(int argc, const char *argv[])
{
    std::thread threads1[10];

    for (int i = 0; i < 10; ++i)
    {
        threads1[i] = std::thread(attempt_10k_increases1);
    }

    for (auto &th : threads1)
    {
        th.join();
    }
    std::cout << counter << " successful increases of the counter.\n";

    return 0;
}
