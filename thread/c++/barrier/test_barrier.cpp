//
// Created by songbinbin on 2020/7/2.
//

#include <iostream>
#include <thread>
#include "barrier.h"

#define THREAD_NUM (10)

swp::barrier g_barrier(THREAD_NUM + 1);

void do_print_id(int id)
{
    // 线程被唤醒, 继续往下执行打印线程编号id.
    std::cout << "thread ok " << id << '\n';

    g_barrier.wait();

    std::cout << "thread done " << id << '\n';
}

int main()
{
    std::thread threads[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = std::thread(do_print_id, i);
    }

    g_barrier.wait();
    std::cout << "10 threads ready ok\n";

    for (auto &th : threads)
    {
        th.join();
    }

    return 0;
}
