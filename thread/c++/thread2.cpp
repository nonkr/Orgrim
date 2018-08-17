/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
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

using namespace std;

void f1(int n)
{
    for (int i = 0; i < 5; ++i)
    {
        cout << "Thread " << n << " executing\n";
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void f2(int &n)
{
    for (int i = 0; i < 5; ++i)
    {
        cout << "Thread 2 executing\n";
        ++n;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main()
{
    int         n = 0;
    thread t1; // t1 is not a thread
    thread t2(f1, n + 1); // pass by value
    thread t3(f2, ref(n)); // pass by reference
    thread t4(move(t3)); // t4 is now running f2(). t3 is no longer a thread

    thread::id t2_id = t2.get_id();
    thread::id t4_id = t4.get_id();

    cout << "t2's id: " << t2_id << '\n';
    cout << "t4's id: " << t4_id << '\n';

    t2.join();
    t4.join();
    cout << "Final value of n is " << n << '\n';
}
