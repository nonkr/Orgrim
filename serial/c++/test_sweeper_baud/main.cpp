//
// Created by songbinbin on 2020/6/2.
//

#include <unistd.h>
#include "McuComm.h"

int main()
{
    Host2McuSensorReg stHost2McuSensorReg = Host2McuSensorReg();

    mcu_comm::Init();

    while (true)
    {
//        usleep(40000);
//        mcu_comm::SetSensorReg(stHost2McuSensorReg);
        sleep(1);
    }
    return 0;
}
