/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/9/1 14:04
 *
 */

#include "MemoryPool.h"

int main()
{
    CMemoryPool *cMemoryPool = new CMemoryPool(256, 100);

    char *p1 = (char *) cMemoryPool->New(6);
    char *p2 = (char *) cMemoryPool->New(6);
    char *p3 = (char *) cMemoryPool->New(6);
    char *p4 = (char *) cMemoryPool->New(6);

    cMemoryPool->Delete(p1);
    cMemoryPool->Delete(p2);
    cMemoryPool->Delete(p3);
    cMemoryPool->Delete(p4);

    delete cMemoryPool;
    return 0;
}
