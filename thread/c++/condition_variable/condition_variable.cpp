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

std::mutex              mtx;            // 全局互斥锁
std::condition_variable cv;             // 全局条件变量
bool                    ready = false;  // 全局标志位

void do_print_id(int id)
{
    std::unique_lock<std::mutex> lck(mtx);

    while (!ready) // 如果标志位不为 true, 则等待...
    {
        cv.wait(lck); // 当前线程被阻塞, 当全局标志位变为 true 之后,
    }

    // 线程被唤醒, 继续往下执行打印线程编号id.
    std::cout << "thread " << id << '\n';
}

void go()
{
    std::unique_lock<std::mutex> lck(mtx);
    ready = true; // 设置全局标志位为 true.
    cv.notify_all(); // 唤醒所有线程.
}

int main()
{
    std::thread threads[10];

    for (int i = 0; i < 10; ++i)
    {
        threads[i] = std::thread(do_print_id, i);
    }

    std::cout << "10 threads ready to race...\n";
    go();

    for (auto &th : threads)
    {
        th.join();
    }

    return 0;
}
