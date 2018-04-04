/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/30 16:54
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

/* Flag set by ‘--verbose’. */
static int verbose_flag;

int
main(int argc, char *argv[])
{
    int c;

    static struct option long_options[] =
                             {
                                 /* These options set a flag. */
                                 {"verbose", no_argument, &verbose_flag, 1},
                                 {"brief",   no_argument, &verbose_flag, 0},
                                 /* These options don’t set a flag.
                                    We distinguish them by their indices. */
                                 {"add",     no_argument,       NULL,    'a'},
                                 {"append",  no_argument,       NULL,    'b'},
                                 {"delete",  required_argument, NULL,    'd'},
                                 {"create",  required_argument, NULL,    'c'},
                                 {"file",    required_argument, NULL,    'f'},
                                 {0, 0,                         NULL,    0}
                             };

    // 控制是否向STDERR打印错误。为0时则关闭打印
    opterr = 0;

    /*
     * 全局变量optopt用于存储出错的option（如缺参数），或者不认识的option
     */

    /* getopt_long stores the option index here. */
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "abc:d:f:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 'a':
                printf("option -a\n");
                break;

            case 'b':
                printf("option -b\n");
                break;

            case 'c':
                printf("option -c with value '%s'\n", optarg);
                break;

            case 'd':
                printf("option -d with value '%s'\n", optarg);
                break;

            case 'f':
                printf("option -f with value '%s'\n", optarg);
                break;

            case '?':
                switch (optopt)
                {
                    case 'c':
                    case 'd':
                    case 'f':
                        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                        break;
                    default:
                        if (isprint(optopt))
                            fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                        else
                            fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }

    /* Instead of reporting ‘--verbose’
       and ‘--brief’ as they are encountered,
       we report the final status resulting from them. */
    if (verbose_flag)
        puts("verbose flag is set");

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        putchar('\n');
    }

    exit(0);
}
