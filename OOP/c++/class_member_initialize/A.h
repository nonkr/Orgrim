/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/9/13 18:51
 *
 */

#ifndef __ORGRIM_A_H__
#define __ORGRIM_A_H__

#include <cstdio>
#include "B.h"

class A
{
public:
    A()
    {
        printf("A c'tor\n");
    }

    ~A()
    {
        printf("A d'tor\n");
    }

    void Func()
    {

    }

private:
    B b;
};

#endif // __ORGRIM_A_H__
