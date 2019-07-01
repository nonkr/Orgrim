/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/17 17:11
 *
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

std::mutex              mtx;            // 全局互斥锁
std::condition_variable cv;             // 全局条件变量
bool                    ready = false;  // 全局标志位

void th1(int id)
{
    std::cout << "th1" << std::endl;

    { // 这对大括号可以影响lck的生命周期，从而影响mtx的锁释放
        std::unique_lock<std::mutex> lck(mtx);

        while (!ready)
        {
            cv.wait(lck);
        }
    }

    sleep(2);

    std::cout << "th1 done" << std::endl;
}

void go()
{
    std::cout << "go" << std::endl;
    std::unique_lock<std::mutex> lck(mtx);
    ready = true;

    sleep(2);
    cv.notify_all();
    std::cout << "go done" << std::endl;
}

void go2()
{
    std::cout << "go2" << std::endl;
    std::unique_lock<std::mutex> lck(mtx);
    std::cout << "go2 done" << std::endl;
}

int main()
{
    std::thread thrd = std::thread(th1, 0);
    sleep(1);

    go();

    sleep(1);
    go2();

    if (thrd.joinable())
    {
        thrd.join();
    }

    return 0;
}
