/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/9 10:21
 *
 */

#include <unistd.h>
#include "Progress.h"

int main()
{
    Progress p;

    for (int i = 0; i <= 123; i++)
    {
        p.Display("Label", 123, i);
        usleep(10000);
    }

    p.Reset();
    for (int i = 0; i <= 512; i++)
    {
        p.Display("Label2", 512, i);
        usleep(10000);
    }

    return 0;
}
