/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/17 13:55
 *
 */

#ifndef ORGRIM_ARGS_H
#define ORGRIM_ARGS_H

/* expands to the first argument */
#define FIRST_ARG(...) FIRST_ARG_HELPER(__VA_ARGS__, throwaway)
#define FIRST_ARG_HELPER(first, ...) first

#define TWO_ARG(...) TWO_ARG_HELPER(__VA_ARGS__, throwaway)
#define TWO_ARG_HELPER(first, two, ...) first, two

/*
 * if there's only one argument, expands to nothing.  if there is more
 * than one argument, expands to a comma followed by everything but
 * the first argument.  only supports up to 29 arguments but can be
 * trivially expanded.
 */
#define REST_ARGS(...) REST_ARGS_HELPER(NUM(__VA_ARGS__), __VA_ARGS__)
#define REST_ARGS_HELPER(qty, ...) REST_ARGS_HELPER2(qty, __VA_ARGS__)
#define REST_ARGS_HELPER2(qty, ...) REST_ARGS_HELPER_##qty(__VA_ARGS__)
#define REST_ARGS_HELPER_ONE(first)
#define REST_ARGS_HELPER_TWOORMORE(first, ...) , __VA_ARGS__
#define NUM(...) \
    SELECT_20TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
#define SELECT_20TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, \
                    a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, \
                    a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, ...) a30

#endif //ORGRIM_ARGS_H
