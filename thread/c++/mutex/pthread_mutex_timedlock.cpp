#include <iostream>
#include <thread>

#include "../../../common/Locker.h"

locker lck;

void f()
{
    if (!lck.trylockfor(2))
    {
        std::cout << "hello world failed\n";
    }
    else
    {
        std::cout << "hello world\n";
    }
}

int main()
{
    lck.lock();
    std::thread t(f);
    t.join();
    return 1;
}
