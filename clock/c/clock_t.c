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
#include <unistd.h>

int main()
{
    // 使用clock()构造clock_t对象(实际上是long类型的变量)
    clock_t t1 = clock();

    // 一段计算
//    for (int i = 0; i < 1000000; i++)
//    {
//        pow(2, i);
//    }
    sleep(1);

    // 计算clock差，除以clock数/每秒，即可求出秒数
    printf("%f\n", (double) (clock() - t1) / CLOCKS_PER_SEC);

    clock_t old = 0;
    clock_t tmp;

    while (1)
    {
        sleep(1);
        if (old == 0)
        {
            old = clock();
        }
        else
        {
            tmp = clock();
            printf("### %f s\n", (tmp - old) * 1.0 / CLOCKS_PER_SEC);
            old = tmp;
        }
    }

    return 0;
}
