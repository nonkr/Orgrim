/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/16 17:09
 *
 */

#include <stdio.h>

void func(int *pInt)
{
    *pInt = 1;
}


int main()
{
    int i = 0;

    printf("i:[%d]\n", i);
    func(&i);
    printf("i:[%d]\n", i);

    return 0;
}
