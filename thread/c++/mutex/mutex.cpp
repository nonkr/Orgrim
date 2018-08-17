/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/17 14:40
 *
 */

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

volatile int counter1(0);    // non-atomic counter1
volatile int counter2(0);    // non-atomic counter2
mutex        mtx1;           // locks access to counter1
mutex        mtx2;           // locks access to counter2

void attempt_10k_increases1()
{
    for (int i = 0; i < 10000; ++i)
    {
        if (mtx1.try_lock())
        {   // only increase if currently not locked:
            ++counter1;
            mtx1.unlock();
        }
    }
}

void attempt_10k_increases2()
{
    for (int i = 0; i < 10000; ++i)
    {
        mtx2.lock();
        ++counter2;
        mtx2.unlock();
    }
}

int main(int argc, const char *argv[])
{
    thread threads1[10];
    thread threads2[10];

    for (int i = 0; i < 10; ++i)
    {
        threads1[i] = thread(attempt_10k_increases1);
        threads2[i] = thread(attempt_10k_increases2);
    }

    for (auto &th : threads1)
    {
        th.join();
    }
    for (auto &th : threads2)
    {
        th.join();
    }
    cout << counter1 << " successful increases of the counter1.\n";
    cout << counter2 << " successful increases of the counter2.\n";

    return 0;
}
