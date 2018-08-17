/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/17 13:52
 *
 */

#include <chrono>
#include <iostream>
#include <future>

using namespace std;

int main()
{
    future<int> f1 = async(launch::async, []()
    {
        std::chrono::milliseconds dura(2000);
        std::this_thread::sleep_for(dura);
        return 0;
    });
    future<int> f2 = async(launch::async, []()
    {
        std::chrono::milliseconds dura(2000);
        std::this_thread::sleep_for(dura);
        return 1;
    });
    cout << "Results are: "
         << f1.get() << " " << f2.get() << "\n";
    return 0;
}
