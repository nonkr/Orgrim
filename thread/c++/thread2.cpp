/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/29 19:33
 *
 */

#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

void f1(int n)
{
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "Thread " << n << " executing\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void f2(int &n)
{
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "Thread 2 executing\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    int         n = 0;
    std::thread t1; // t1 is not a thread
    std::thread t2(f1, n + 1); // pass by value
    std::thread t3(f2, std::ref(n)); // pass by reference
    std::thread t4(std::move(t3)); // t4 is now running f2(). t3 is no longer a thread

    std::thread::id t2_id = t2.get_id();
    std::thread::id t4_id = t4.get_id();

    std::cout << "t2's id: " << t2_id << '\n';
    std::cout << "t4's id: " << t4_id << '\n';

    t2.join();
    t4.join();
    std::cout << "Final value of n is " << n << '\n';
}