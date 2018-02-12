/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/21 0:32
 *
 */

#include <stdlib.h>

#include <chrono>    // std::chrono::seconds
#include <iostream>  // std::cout
#include <thread>    // std::thread, std::this_thread::sleep_for

void thread_task(int n)
{
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::cout << "hello thread "
              << std::this_thread::get_id()
              << " paused " << n << " seconds" << std::endl;
}

int main(int argc, const char *argv[])
{
    std::thread threads[5];
    std::cout << "Spawning 5 threads...\n";
    for (int i = 0; i < 5; i++)
    {
        threads[i] = std::thread(thread_task, i + 1);
    }
    std::cout << "Done spawning threads! Now wait for them to join\n";
    for (auto &t: threads)
    {
        t.join();
    }
    std::cout << "All threads joined.\n";

    return EXIT_SUCCESS;
}
