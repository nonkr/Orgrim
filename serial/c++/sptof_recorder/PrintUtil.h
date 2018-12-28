#ifndef _PRINTUTIL_H_
#define _PRINTUTIL_H_

#include <cstdio>

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
    SELECT_20TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, \
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
#define SELECT_20TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, \
                    a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, \
                    a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, ...) a30

#define FR_PRINT(...)                   printf(FIRST_ARG(__VA_ARGS__) REST_ARGS(__VA_ARGS__))
#define FR_PRINT_RED(...)               printf(KRED FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_SALMON(...)            printf(KSALMON FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_GREEN(...)             printf(KGREEN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_LIMEGREEN(...)         printf(KLIMEGREEN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_YELLOW(...)            printf(KYELLOW FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_ORANGE(...)            printf(KORANGE FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_BLUE(...)              printf(KBLUE FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_MAGENTA(...)           printf(KMAGENTA FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_PINK(...)              printf(KPINK FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_CYAN(...)              printf(KCYAN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))
#define FR_PRINT_RED_DETAIL(...)        printf("[%s:%u, %s()] " KRED FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_SALMON_DETAIL(...)     printf("[%s:%u, %s()] " KSALMON FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_GREEN_DETAIL(...)      printf("[%s:%u, %s()] " KGREEN FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_LIMEGREEN_DETAIL(...)  printf("[%s:%u, %s()] " KLIMEGREEN FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_YELLOW_DETAIL(...)     printf("[%s:%u, %s()] " KYELLOW FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_ORANGE_DETAIL(...)     printf("[%s:%u, %s()] " KORANGE FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_BLUE_DETAIL(...)       printf("[%s:%u, %s()] " KBLUE FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_MAGENTA_DETAIL(...)    printf("[%s:%u, %s()] " KMAGENTA FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_PINK_DETAIL(...)       printf("[%s:%u, %s()] " KPINK FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_CYAN_DETAIL(...)       printf("[%s:%u, %s()] " KCYAN FIRST_ARG(__VA_ARGS__) KRESET,__FILE__,__LINE__,__func__ REST_ARGS(__VA_ARGS__))

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
