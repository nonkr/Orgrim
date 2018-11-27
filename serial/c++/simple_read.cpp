/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/11/27 14:30
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "serial.h"
#include "TimeUtil.h"

int main(int argc, char **argv)
{
    int        fd;
    char       buf[1024];
    int        iReadLen     = 0;
    SerialOpts stSerialOpts = {
        .pDevice   = (char *) "/dev/ttyS1",
        .nSpeed    = 460800,
        .nDatabits = 8,
        .nStopbits = 1,
        .nParity   = 'n',
    };

    printf("use tty:%s baud:%d\n", stSerialOpts.pDevice, stSerialOpts.nSpeed);

    if ((fd = open_serial(&stSerialOpts)) < 0)
    {
        fprintf(stderr, "open serial failed\n");
        exit(1);
    }

    system_clock::time_point old = system_clock::now();

    while (true)
    {
        iReadLen = read(fd, buf, 1024);
        system_clock::time_point now      = system_clock::now();
        auto                     duration = duration_cast<microseconds>(now - old);
        double                   s        =
                                     double(duration.count()) * microseconds::period::num / microseconds::period::den;
        old = now;
//        if (s > 0.025)
//        {
//            FR_PRINT_MAGENTA("Time duration of read: %f sec.\n", s);
//        }
        if (iReadLen <= 36)
        {
//            printf("%d\n", iReadLen);
        }
        else
        {
            printf("0x%02X 0x%02X 0x%02X\n", buf[iReadLen - 36], buf[iReadLen - 2], buf[iReadLen - 1]);
            FR_PRINT_RED("%d\n", iReadLen);
        }
    }

    close_serial(fd);

    return 0;
}
