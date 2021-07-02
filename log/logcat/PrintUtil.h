/*
 * Copyright: Hikvision Software Co.Ltd. All Rights Reserved.
 * File: PrintUtil.h
 * Description: Print utils.
 * Author: songbinbin
 * Date: 2020/05/26
 * Changelog:
 *   v1.0 2018/05/10 first revision.
 *   v2.0 2018/07/06 remove colorful printf, use ALOG instead.
 *   v2.1 2018/07/17 add magic macro, FIRST_ARG and REST_ARGS.
 *   v2.2 2018/07/20 add printf macro for debug.
 *   v2.3 2020/05/26 add __FILE__ and __LINE__.
 */

#ifndef _PRINTUTIL_H_
#define _PRINTUTIL_H_

#include <cstdio>
#include <cstring>
#include <libgen.h>
#include "log/log.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define COLORIZE_OUTPUT
#define FR_PRINT_TO_ALOG

#ifdef COLORIZE_OUTPUT
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
#else
#define KRED        ""
#define KSALMON     ""
#define KGREEN      ""
#define KLIMEGREEN  ""
#define KYELLOW     ""
#define KORANGE     ""
#define KBLUE       ""
#define KMAGENTA    ""
#define KPINK       ""
#define KCYAN       ""
#define KRESET      ""
#endif

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
                      ((byte) & 0x80 ? '1' : '0'), \
                      ((byte) & 0x40 ? '1' : '0'), \
                      ((byte) & 0x20 ? '1' : '0'), \
                      ((byte) & 0x10 ? '1' : '0'), \
                      ((byte) & 0x08 ? '1' : '0'), \
                      ((byte) & 0x04 ? '1' : '0'), \
                      ((byte) & 0x02 ? '1' : '0'), \
                      ((byte) & 0x01 ? '1' : '0')

#define SHORT_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN "" BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY_PRETTY_PATTERN BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY(s) BYTE_TO_BINARY((s) >> 8), BYTE_TO_BINARY(s)

#define INT_TO_BINARY_PATTERN SHORT_TO_BINARY_PATTERN "" SHORT_TO_BINARY_PATTERN
#define INT_TO_BINARY_PRETTY_PATTERN SHORT_TO_BINARY_PRETTY_PATTERN " " SHORT_TO_BINARY_PRETTY_PATTERN
#define INT_TO_BINARY(i) BYTE_TO_BINARY((i) >> 24), \
                         BYTE_TO_BINARY((i) >> 16), \
                         BYTE_TO_BINARY((i) >> 8), \
                         BYTE_TO_BINARY(i)

#define LONGLONG_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN "" INT_TO_BINARY_PATTERN
#define LONGLONG_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN " " INT_TO_BINARY_PRETTY_PATTERN
#define LONGLONG_TO_BINARY(ll) BYTE_TO_BINARY((ll) >> 56), \
                               BYTE_TO_BINARY((ll) >> 48), \
                               BYTE_TO_BINARY((ll) >> 40), \
                               BYTE_TO_BINARY((ll) >> 32), \
                               BYTE_TO_BINARY((ll) >> 24), \
                               BYTE_TO_BINARY((ll) >> 16), \
                               BYTE_TO_BINARY((ll) >> 8), \
                               BYTE_TO_BINARY(ll)


/* expands to the first argument */
#define FIRST_ARG(...) FIRST_ARG_HELPER(__VA_ARGS__, throwaway)
#define FIRST_ARG_HELPER(first, ...) first

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
    SELECT_60TH(__VA_ARGS__, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
#define SELECT_60TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, \
                    a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, \
                    a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, \
                    a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, \
                    a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, \
                    a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, ...) a60

#ifdef FR_PRINT_TO_ALOG
#define FR_PRINT(tag, ...)                   ALOG(LOG_DEBUG, tag, "[%s:%d] " FIRST_ARG(__VA_ARGS__) ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_RED(tag, ...)               ALOG(LOG_ERROR, tag, KRED "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_SALMON(tag, ...)            ALOG(LOG_DEBUG, tag, KSALMON "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_GREEN(tag, ...)             ALOG(LOG_DEBUG, tag, KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_LIMEGREEN(tag, ...)         ALOG(LOG_DEBUG, tag, KLIMEGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_YELLOW(tag, ...)            ALOG(LOG_DEBUG, tag, KYELLOW "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_ORANGE(tag, ...)            ALOG(LOG_DEBUG, tag, KORANGE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_BLUE(tag, ...)              ALOG(LOG_DEBUG, tag, KBLUE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_MAGENTA(tag, ...)           ALOG(LOG_DEBUG, tag, KMAGENTA "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_PINK(tag, ...)              ALOG(LOG_DEBUG, tag, KPINK "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_CYAN(tag, ...)              ALOG(LOG_DEBUG, tag, KCYAN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG(tag, ...)                    ALOG(LOG_DEBUG, tag, "[%s:%d] " FIRST_ARG(__VA_ARGS__) ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_RED(tag, ...)                ALOG(LOG_ERROR, tag, KRED "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_SALMON(tag, ...)             ALOG(LOG_DEBUG, tag, KSALMON "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_GREEN(tag, ...)              ALOG(LOG_DEBUG, tag, KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_LIMEGREEN(tag, ...)          ALOG(LOG_DEBUG, tag, KLIMEGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_YELLOW(tag, ...)             ALOG(LOG_DEBUG, tag, KYELLOW "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_ORANGE(tag, ...)             ALOG(LOG_DEBUG, tag, KORANGE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_BLUE(tag, ...)               ALOG(LOG_DEBUG, tag, KBLUE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_MAGENTA(tag, ...)            ALOG(LOG_DEBUG, tag, KMAGENTA "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_PINK(tag, ...)               ALOG(LOG_DEBUG, tag, KPINK "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG_CYAN(tag, ...)               ALOG(LOG_DEBUG, tag, KCYAN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#else
#define FR_PRINT(tag, ...)                   printf("[%s:%d] " FIRST_ARG(__VA_ARGS__) ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_RED(tag, ...)               printf(KRED "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_SALMON(tag, ...)            printf(KSALMON "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_GREEN(tag, ...)             printf(KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_LIMEGREEN(tag, ...)         printf(KLIMEGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_YELLOW(tag, ...)            printf(KYELLOW "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_ORANGE(tag, ...)            printf(KORANGE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_BLUE(tag, ...)              printf(KBLUE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_MAGENTA(tag, ...)           printf(KMAGENTA "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_PINK(tag, ...)              printf(KPINK "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_CYAN(tag, ...)              printf(KCYAN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_ALOG(tag, ...) do { \
                    printf("[%s:%d] " FIRST_ARG(__VA_ARGS__) ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, "[%s:%d] " FIRST_ARG(__VA_ARGS__) ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_RED(tag, ...) do { \
                    printf(KRED "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_ERROR, tag, KRED "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_SALMON(tag, ...) do { \
                    printf(KSALMON "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KSALMON "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_GREEN(tag, ...) do { \
                    printf(KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_LIMEGREEN(tag, ...) do { \
                    printf(KLIMEGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KLIMEGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_YELLOW(tag, ...) do { \
                    printf(KYELLOW "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KYELLOW "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_ORANGE(tag, ...) do { \
                    printf(KORANGE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KORANGE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_BLUE(tag, ...) do { \
                    printf(KBLUE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KBLUE "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_MAGENTA(tag, ...) do { \
                    printf(KMAGENTA "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KMAGENTA "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_PINK(tag, ...) do { \
                    printf(KPINK "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KPINK "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#define FR_ALOG_CYAN(tag, ...) do { \
                    printf(KCYAN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                    ALOG(LOG_DEBUG, tag, KCYAN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__)); \
                 } while(false);
#endif

class PrintUtil
{
public:
    static void PrintAsHexString(const char *pData, size_t nSize)
    {
        size_t i;
        for (i = 0; i < nSize; i++)
        {
            if (i == nSize - 1)
            {
                printf("%02X", pData[i] & 0xFF);
            }
            else
            {
                printf("%02X ", pData[i] & 0xFF);
            }
        }
        printf("\n");
    }
};

#endif //_PRINTUTIL_H_
