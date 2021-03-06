/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 *
 */

#ifndef CAT_COLOR_H
#define CAT_COLOR_H

#include <stdio.h>
#include "macro/args/c/args.h"

#define KRED        "\x1B[0;31m"    // ANSI red color
#define KSALMON     "\x1B[1;31m"    // ANSI salmon color
#define KGREEN      "\x1B[0;32m"    // ANSI green color
#define KLIMEGREEN  "\x1B[1;32m"    // ANSI limegreen color
#define KYELLOW     "\x1B[0;33m"    // ANSI yellow color
#define KORANGE     "\x1B[0;34m"    // ANSI orange color
#define KBLUE       "\x1B[1;34m"    // ANSI blue color
#define KMAGENTA    "\x1B[0;35m"    // ANSI magenta color
#define KPINK       "\x1B[1;35m"    // ANSI pink color
#define KCYAN       "\x1B[0;36m"    // ANSI cyan color
#define KRESET      "\x1B[0m"       // ANSI reset color

#ifdef ORGRIM_DEBUG
#define OGM_PRINT(...)                  printf(__VA_ARGS__)
#define OGM_PRINT_RED(...)              printf(KRED FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_SALMON(...)           printf(KSALMON FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_GREEN(...)            printf(KGREEN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_LIMEGREEN(...)        printf(KLIMEGREEN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_YELLOW(...)           printf(KYELLOW FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_ORANGE(...)           printf(KORANGE FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_BLUE(...)             printf(KBLUE FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_MAGENTA(...)          printf(KMAGENTA FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_PINK(...)             printf(KPINK FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_CYAN(...)             printf(KCYAN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_DETAIL(...)           printf("[%s:%u, %s()] " FIRST_ARG(__VA_ARGS__), __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_RED_DETAIL(...)       printf("[%s:%u, %s()] " KRED FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_SALMON_DETAIL(...)    printf("[%s:%u, %s()] " KSALMON FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_GREEN_DETAIL(...)     printf("[%s:%u, %s()] " KGREEN FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_LIMEGREEN_DETAIL(...) printf("[%s:%u, %s()] " KLIMEGREEN FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_YELLOW_DETAIL(...)    printf("[%s:%u, %s()] " KYELLOW FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_ORANGE_DETAIL(...)    printf("[%s:%u, %s()] " KORANGE FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_BLUE_DETAIL(...)      printf("[%s:%u, %s()] " KBLUE FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_MAGENTA_DETAIL(...)   printf("[%s:%u, %s()] " KMAGENTA FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_PINK_DETAIL(...)      printf("[%s:%u, %s()] " KPINK FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define OGM_PRINT_CYAN_DETAIL(...)      printf("[%s:%u, %s()] " KCYAN FIRST_ARG(__VA_ARGS__) KRESET, __FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#else
#define OGM_PRINT(...)
#define OGM_PRINT_RED(...)
#define OGM_PRINT_SALMON(...)
#define OGM_PRINT_GREEN(...)
#define OGM_PRINT_LIMEGREEN(...)
#define OGM_PRINT_YELLOW(...)
#define OGM_PRINT_ORANGE(...)
#define OGM_PRINT_BLUE(...)
#define OGM_PRINT_MAGENTA(...)
#define OGM_PRINT_PINK(...)
#define OGM_PRINT_CYAN(...)
#define OGM_PRINT_DETAIL(...)
#define OGM_PRINT_RED_DETAIL(...)
#define OGM_PRINT_SALMON_DETAIL(...)
#define OGM_PRINT_GREEN_DETAIL(...)
#define OGM_PRINT_LIMEGREEN_DETAIL(...)
#define OGM_PRINT_YELLOW_DETAIL(...)
#define OGM_PRINT_ORANGE_DETAIL(...)
#define OGM_PRINT_BLUE_DETAIL(...)
#define OGM_PRINT_MAGENTA_DETAIL(...)
#define OGM_PRINT_PINK_DETAIL(...)
#define OGM_PRINT_CYAN_DETAIL(...)
#endif

#endif //CAT_COLOR_H
