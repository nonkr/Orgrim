#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

void ThreadRoutine()
{
    this_thread::sleep_for(chrono::seconds(1));
    cout << "hello thread "
         << this_thread::get_id()
         << " paused " << 1 << " seconds" << endl;
}

int main()
{
    thread thrd(ThreadRoutine);
    if (!thrd.joinable())
    {
        printf("thread create failed!\n");
        return 1;
    }

    thrd.join();

    return 0;
}
