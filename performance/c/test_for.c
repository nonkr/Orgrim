/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/5/9 17:10
 *
 */

#define TOF_PIXELS (320*240)
#define TOFDATASIZE (TOF_PIXELS*4)

int main()
{
    int i, j, k;

    for (i = 0, k = 0; i < TOFDATASIZE; i += 32)
    {
        for (j = 0; j < 16; j += 2)
        {
            k++;
        }
    }

    return 0;
}
