/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/11/12 21:19
 *
 */

#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include "IMURequester.h"
#include "SPToF.h"

IMURequester *pIMURequester;
SPToF        *pSPToF;
bool         G_RUNNING = true;

void SignalHandler(int signum)
{
    G_RUNNING = false;
    pIMURequester->Stop();
}

int main()
{
    signal(SIGINT, SignalHandler);

    IMU_t stIMU        = {0};
    short sMiddleSPToF = 8888;

    pIMURequester = new IMURequester();

    pSPToF = new SPToF;
    if (pSPToF->Init(0, 0))
    {
        return 1;
    }

    while (G_RUNNING)
    {
        pIMURequester->GetIMU(&stIMU);
        sMiddleSPToF = pSPToF->GetSPToFDistance();
        printf("%f,%f,%d,%d,%d,%d\n", stIMU.x, stIMU.y, stIMU.degree, sMiddleSPToF, stIMU.leftSingleToF, stIMU.rightSingleToF);
    }

    delete pIMURequester;
    pSPToF->DeInit();
    delete pSPToF;
}
