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

IMURequester *pIMURequester;
bool         G_RUNNING = true;

void SignalHandler(int signum)
{
    G_RUNNING = false;
    pIMURequester->Stop();
}

int main()
{
    signal(SIGINT, SignalHandler);

    IMU_t         stIMU = {0};
    unsigned char index = 0;

    pIMURequester = new IMURequester();

    printf("\x1B[?25l");
    while (G_RUNNING)
    {
        pIMURequester->GetIMU(&stIMU, index++);
        usleep(50000);
    }

    delete pIMURequester;
    printf("\x1B[?25h\n");
}
