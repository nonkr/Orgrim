/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/5/21 10:21
 *
 */

#include <stdio.h>
#include <math.h>

int main()
{
    float          val1, val2, val3, val4;
    unsigned short val5, val6;

    val1 = 1.6;
    val2 = 1.2;
    val3 = 2.8;
    val4 = 2.3;
    val5 = 8888;
    val6 = 3276;

    printf("value1 = %.1lf\n", ceil(val1));
    printf("value2 = %.1lf\n", ceil(val2));
    printf("value3 = %.1lf\n", ceil(val3));
    printf("value4 = %.1lf\n", ceil(val4));
    printf("value5 = %.1lf\n", ceil((float) val5 / 10));
    printf("value6 = %.1lf\n", ceil((float) val6 / 10));
    printf("value6 = %u\n", (unsigned char) ceil((float) val6 / 10));

    return (0);
}
