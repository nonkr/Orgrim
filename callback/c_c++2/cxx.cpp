/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/25 17:19
 *
 */

#include <cstdio>
#include "cxx.h"
#include "c.h"

int CXX::Callback(int i)
{
    printf("i:%d\n", i);
    return 0;
}

CXX::CXX()
{
    StA *pStA = new StA;

    pStA->fp = CXX::Callback;

    Setup(pStA);

    delete pStA;
}
