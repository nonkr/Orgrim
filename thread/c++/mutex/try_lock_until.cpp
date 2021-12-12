#include <iostream>
#include <chrono>

#include <thread>
#include <mutex>

std::timed_mutex test_mutex;

void f()
{
    auto now = std::chrono::steady_clock::now();
    test_mutex.try_lock_until(now + std::chrono::seconds(2));
    std::cout << "hello world\n";
}

int main()
{
    std::lock_guard<std::timed_mutex> l(test_mutex);
    std::thread t(f);
    t.join();
}
