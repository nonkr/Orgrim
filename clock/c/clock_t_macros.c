/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/22 15:45
 *
 */

#include <time.h>
#include <math.h>
#include <stdio.h>

#define TICK(X) clock_t X = clock()
#define TOCK(X) printf("time %s: %g sec.\n", (#X), (double)(clock() - (X)) / CLOCKS_PER_SEC)

int main()
{
    TICK(aa);

    // 一段计算
    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }

    TOCK(aa);

    return 0;
}