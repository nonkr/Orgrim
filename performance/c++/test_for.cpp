/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/7/5 12:40
 *
 */

#include "TimeUtil.h"

void test_for()
{
    unsigned int  i, j, k;
    unsigned int  iToFPixels   = 320 * 240;
    unsigned int  iToFDataSize = 320 * 240 * 4;
    short         sAmplitudeNormal[320 * 240] __attribute__((__unused__));
    short         sPhaseNormal[320 * 240]  __attribute__((__unused__));
    short         sFlagNormal[320 * 240]   __attribute__((__unused__));
    unsigned char raw[320 * 240 * 4 * 2];

    for (i = 0, k = 0; i < iToFDataSize; i += 32)
    {
        for (j = 0; j < 16; j += 2)
        {
            sAmplitudeNormal[iToFPixels - 1 - k] = (short) ((raw[(i + j) * 2]) |
                                                            (raw[(i + j + 1) * 2] << 8 & 0x0F00));
            sPhaseNormal[iToFPixels - 1 - k]     = (short) ((raw[(i + j + 16) * 2]) |
                                                            (raw[(i + j + 1 + 16) * 2] << 8 &
                                                             0x0F00));
            sFlagNormal[iToFPixels - 1 - k]      = (short) (raw[(i + j + 1 + 16) * 2] >> 7);
            k++;
        }
    }
}

int main(int argc, char *argv[])
{
    TICK(x)
    for (int i = 0; i < 10000000; i++)
    {
//        printf("%d\n", i);
        test_for();
    }
    TOCK(x, "for")
    return 0;
}
