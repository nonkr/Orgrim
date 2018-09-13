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

#ifndef __ORGRIM_B_H__
#define __ORGRIM_B_H__

#include <cstdio>

class B
{
public:
    B()
    {
        printf("B c'tor\n");
    }

    ~B()
    {
        printf("B d'tor\n");
    }
};

#endif // __ORGRIM_B_H__
