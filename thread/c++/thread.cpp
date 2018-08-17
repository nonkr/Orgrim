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

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

void thread_task(int n)
{
    this_thread::sleep_for(chrono::seconds(n));
    cout << "hello thread "
         << this_thread::get_id()
         << " paused " << n << " seconds" << endl;
}

int main(int argc, const char *argv[])
{
    thread threads[5];
    cout << "Spawning 5 threads...\n";
    for (int i = 0; i < 5; i++)
    {
        threads[i] = thread(thread_task, i + 1);
    }
    cout << "Done spawning threads! Now wait for them to join\n";
    for (auto &t: threads)
    {
        t.join();
    }
    cout << "All threads joined.\n";

    return EXIT_SUCCESS;
}
