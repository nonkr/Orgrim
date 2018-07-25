/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/17 12:51
 *
 */

#include <stdio.h>
#include "args.h"

#define BAR(...) printf(FIRST_ARG(__VA_ARGS__)REST_ARGS(__VA_ARGS__))

int main(int argc, char *argv[])
{
    BAR("first test\n");
    BAR("second test: %s %d %d %d %d %d %d %d %d %d %d %d\n", "a string", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
    return 0;
}