#pragma once

#include "McuProtocol.h"
#include <string>

#define Sleep(X) std::this_thread::sleep_for(std::chrono::milliseconds((int)(X)))

namespace mcu_comm
{
    bool Init();

    bool DeInit();
}
