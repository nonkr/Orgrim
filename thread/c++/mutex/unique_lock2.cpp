/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2020/7/2 11:14
 *
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>

std::mutex mtx;

void print_block1()
{
    std::cout << "first enter" << std::endl;
    {
        std::unique_lock<std::mutex> lck(mtx);
        std::cout << "scope enter" << std::endl;
        sleep(2);
        std::cout << "first unlock()" << std::endl;
        lck.unlock(); // 可以对unique_lock手动执行解锁

        sleep(1);
        std::cout << "first lock()" << std::endl;
        lck.lock(); // 可以对unique_lock手动执行上锁，退出时也会自动释放锁
        std::cout << "scope leave" << std::endl;
    }

//    mtx.lock(); // 这里如果通过这种方式上锁，不会自动解锁
    std::cout << "first leave" << std::endl;
}

void print_block2()
{
    mtx.lock();
    std::cout << "second enter" << std::endl;
    std::cout << 2 << std::endl;
    sleep(2);
    std::cout << "second leave" << std::endl;
    mtx.unlock();
}

void print_block3()
{
    std::unique_lock<std::mutex> lck(mtx);
    std::cout << "third enter" << std::endl;
    std::cout << 3 << std::endl;
    sleep(2);
    std::cout << "third leave" << std::endl;
}

int main()
{
    std::thread th1(print_block1);

    sleep(1);

    std::thread th2(print_block2);

    sleep(6);
    std::thread th3(print_block3);

    th1.join();
    th2.join();
    th3.join();

    return 0;
}
