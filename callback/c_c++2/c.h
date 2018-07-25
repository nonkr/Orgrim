/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/25 17:20
 *
 */

#ifndef ORGRIM_C_H
#define ORGRIM_C_H

#include <stdio.h>

__BEGIN_DECLS

typedef struct
{
    int (*fp)(int);
} StA;

void Setup(StA *pStA);

__END_DECLS

#endif //ORGRIM_C_H
