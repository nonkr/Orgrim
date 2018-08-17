/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/17 15:33
 *
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <stdexcept>

std::mutex mtx;

void print_even(int x)
{
    if (x % 2 == 0)
    {
        std::cout << x << " is even\n";
    }
    else
    {
        throw (std::logic_error("not even"));
    }
}

void print_thread_id(int id)
{
    try
    {
        // using a local lock_guard to lock mtx guarantees unlocking on destruction / exception:
        std::lock_guard<std::mutex> lck(mtx);
        print_even(id);
    }
    catch (std::logic_error &)
    {
        std::cout << "[exception caught]\n";
    }
}

int main()
{
    std::thread threads[10];

    for (int i = 0; i < 10; ++i)
    {
        threads[i] = std::thread(print_thread_id, i + 1);
    }

    for (auto &th : threads)
    {
        th.join();
    }

    return 0;
}
