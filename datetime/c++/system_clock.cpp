#include "../../common/TimeUtil.h"

int main()
{
    while (true)
    {
        system_clock::time_point now_system   = std::chrono::system_clock::now();
        uint64_t                 milli_system = std::chrono::duration_cast<std::chrono::milliseconds>(
            now_system.time_since_epoch()).count();
        printf("milli_system:[%llu]\n", milli_system);

        steady_clock::time_point now_steady   = std::chrono::steady_clock::now();
        uint64_t                 milli_steady = std::chrono::duration_cast<std::chrono::milliseconds>(
            now_steady.time_since_epoch()).count();
        printf("milli_steady:[%llu]\n", milli_steady);
        sleep(1);
    }
    return 0;
}
