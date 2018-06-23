/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/6/23 13:45
 *
 */

#include <stdio.h>
#include "../utils/print_utils.h"

#define DT_EMPTY     (0x0000)
#define DT_A         (0x0001)
#define DT_B         (0x0002)

#define DT_SET_A(x)  ((x)|DT_A)
#define DT_SET_B(x)  ((x)|DT_B)

#define DT_UNSET_A(x)  ((x)^DT_A)
#define DT_UNSET_B(x)  ((x)^DT_B)

#define MT_HAS_A(x)  ((x)&DT_A)
#define MT_HAS_B(x)  ((x)&DT_B)

int main()
{
    int i = 0;

    i = DT_SET_A(i);
    printf("i: [" BYTE_TO_BINARY_PATTERN "]\n", BYTE_TO_BINARY(i));

    i = DT_SET_B(i);
    printf("i: [" BYTE_TO_BINARY_PATTERN "]\n", BYTE_TO_BINARY(i));

    i = DT_UNSET_A(i);
    printf("i: [" BYTE_TO_BINARY_PATTERN "]\n", BYTE_TO_BINARY(i));

    i = DT_UNSET_B(i);
    printf("i: [" BYTE_TO_BINARY_PATTERN "]\n", BYTE_TO_BINARY(i));

    return (0);
}
