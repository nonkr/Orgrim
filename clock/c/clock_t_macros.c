/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/22 15:45
 *
 */

#include <time.h>
#include <math.h>
#include <stdio.h>

#define TICK(S) clock_t S = clock()
#define TOCK(S, E, LABEL) clock_t E = clock(); \
                          printf("Time duration of %s: %g sec.\n", LABEL, (double)((E) - (S)) / CLOCKS_PER_SEC);

int main()
{
    TICK(a);

    // 一段计算
    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }

    TOCK(a, b, "First");

    for (int i = 0; i < 1000000; i++)
    {
        pow(2, i);
    }
    TOCK(b, c, "Second");

    return 0;
}
