/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/17 13:54
 *
 */

#include <atomic>
#include <iostream>
#include <thread>

using namespace std;

const int NUM = 100;

int target = 0;
atomic<int> atomicTarget(0);

template<typename T>
void atomicPlusOne(int trys)
{
    while (trys > 0) {
        atomicTarget.fetch_add(1);
        --trys;
    }
}

void plusOne(int trys)
{
    while (trys > 0) {
        ++target;
        --trys;
    }
}

int main()
{
    thread threads[NUM];
    thread atomicThreads[NUM];
    for (int i = 0; i < NUM; i++) {
        atomicThreads[i] = thread(atomicPlusOne<int>, 10000);
    }
    for (int i = 0; i < NUM; i++) {
        threads[i] = thread(plusOne, 10000);
    }

    for (int i = 0; i < NUM; i++) {
        atomicThreads[i].join();
    }
    for (int i = 0; i < NUM; i++) {
        threads[i].join();
    }

    cout << "Atomic target's value : " << atomicTarget << "\n";
    cout << "Non-atomic target's value : " << target << "\n";
    // atomicTarget的值总是固定的，而target的值每次运行时各不相同
    //
    // g++ -std=c++11 -pthread ./atomic.cpp
    // Atomic target's value : 1000000
    // Non-atomic target's value : 842480
    return 0;
}
