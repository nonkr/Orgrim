/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/29 19:46
 *
 */

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void foo()
{
    this_thread::sleep_for(chrono::seconds(1));
}

void bar()
{
    this_thread::sleep_for(chrono::seconds(1));
}

int main()
{
    thread t1(foo);
    thread t2(bar);

    cout << "thread 1 id: " << t1.get_id() << endl;
    cout << "thread 2 id: " << t2.get_id() << endl;

    swap(t1, t2);

    cout << "after swap(t1, t2):" << endl;
    cout << "thread 1 id: " << t1.get_id() << endl;
    cout << "thread 2 id: " << t2.get_id() << endl;

    t1.swap(t2);

    cout << "after t1.swap(t2):" << endl;
    cout << "thread 1 id: " << t1.get_id() << endl;
    cout << "thread 2 id: " << t2.get_id() << endl;

    t1.join();
    t2.join();
}
