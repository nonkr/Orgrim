/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/6/2 17:11
 *
 */

#include "MPToF.h"

int main()
{
    MPToF *pMPToF = new MPToF();

    pMPToF->Init();

    pMPToF->DeInit();

    delete pMPToF;
    return 0;
}