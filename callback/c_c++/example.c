/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/2/6 16:44
 *
 */

#include <stdio.h>
#include "example.h"

int FuncInC(int (*cb)(void *, int, int), void *pIn, void *context)
{
    struct Param *p = (struct Param *) pIn;
    printf("FuncInC.a:[%d]\n", p->a);
    return cb(context, 0x55AA, p->a);
}