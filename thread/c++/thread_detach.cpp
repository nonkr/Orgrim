/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/29 19:42
 *
 */

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

void independentThread()
{
    cout << "Starting concurrent thread.\n";
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Exiting concurrent thread.\n";
}

void threadCaller()
{
    cout << "Starting thread caller.\n";
    thread t(independentThread);
    t.detach();
//    t.join();
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Exiting thread caller.\n";
}

int main()
{
    threadCaller();
    this_thread::sleep_for(chrono::seconds(5));
}
