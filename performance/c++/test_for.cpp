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

void test1()
{
    FR_DURATION_AVG(x, "test1");
    int r1 = rand() % (10 - 1 + 1) + 1;
    int r;
    if (r1 >= 8)
    {
        r = rand() % (3000 - 100 + 1) + 100;
    }
    else
    {
        r = rand() % (300 - 100 + 1) + 100;
    }
    printf("usleep %d\n", r);
    usleep(r * 1000);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    for (int i = 0; i < 300; i++)
    {
        test1();
    }
    return 0;
}
