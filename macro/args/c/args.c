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

//#define BAR(...) printf(FIRST_ARG(__VA_ARGS__)REST_ARGS(__VA_ARGS__))
#define BAR(...) printf(TWO_ARG(__VA_ARGS__))

#define FOO(x) do { \
                 static int i##x = 0; \
                 if (i##x == 0) \
                 { \
                    i##x = 1; \
                 } \
                 else \
                 { \
                    printf("abc\n"); \
                 } \
               } while (0)

void func()
{
    FOO(1);
}

int main(int argc, char *argv[])
{
    BAR("first test:%s\n", "fcc");
//    BAR("second test: %s %d %d %d %d %d %d %d %d %d %d %d\n", "a string", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);

    func();
    func();
    func();

    return 0;
}
