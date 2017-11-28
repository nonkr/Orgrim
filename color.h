//
// Created by songbinbin on 2017/11/28.
//

#ifndef CAT_COLOR_H
#define CAT_COLOR_H

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
#define KRESET       "\x1B[0m"       // ANSI reset color

#if CAT_DEBUG
#define CAT_PRINT(...) printf(__VA_ARGS__)                           // normal printf
#define CAT_PRINT_DETAIL(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(__VA_ARGS__))                            // print with detail info
#define CAT_PRINT_GREEN(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KGREEN),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print debug info
#define CAT_PRINT_ORANGE(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KORANGE),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print fatal info
#define CAT_PRINT_RED(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KRED),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print emergent info
#define CAT_PRINT_SALMON(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KSALMON),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print alert info
#define CAT_PRINT_MAGENTA(...) (printf("[%s:%u, %s()] ",\
                            __FILE__,__LINE__,__func__),\
                            printf(KMAGENTA),\
                            printf(__VA_ARGS__),\
                            printf(KRESET))                              // print critical info
#define CAT_PRINT_PINK(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KPINK),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print error info
#define CAT_PRINT_YELLOW(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KYELLOW),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print warning info
#define CAT_PRINT_LIMEGREEN(...) (printf("[%s:%u, %s()] ",\
                        __FILE__,__LINE__,__func__),\
                        printf(KLIMEGREEN),\
                        printf(__VA_ARGS__),\
                        printf(KRESET))                                  // print notice info
#define CAT_PRINT_BLUE(...) (printf("[%s:%u, %s()] ",\
                            __FILE__,__LINE__,__func__),\
                            printf(KBLUE),\
                            printf(__VA_ARGS__),\
                            printf(KRESET))                              // print just into
#else
#define CAT_PRINT(...)
#define CAT_PRINT_DETAIL(...)
#define CAT_PRINT_GREEN(...)
#define CAT_PRINT_ORANGE(...)
#define CAT_PRINT_RED(...)
#define CAT_PRINT_SALMON(...)
#define CAT_PRINT_MAGENTA(...)
#define CAT_PRINT_PINK(...)
#define CAT_PRINT_YELLOW(...)
#define CAT_PRINT_LIMEGREEN(...)
#define CAT_PRINT_BLUE(...)
#endif

#endif //CAT_COLOR_H
