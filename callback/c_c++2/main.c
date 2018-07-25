/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/25 18:34
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "c.h"

int Callback(int i)
{
    printf("i:%d\n", i);
    return 0;
}

int main()
{
    StA *pStA = malloc(sizeof(StA));

    pStA->fp = Callback;

    Setup(pStA);

    free(pStA);
    return 0;
}
