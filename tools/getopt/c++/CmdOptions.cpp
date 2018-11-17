#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include "CmdOptions.h"

AppOptions_t G_AppOptions = {
    .nBackground = 0,
    .nVerbose = 0,
    .nArgOnlyLongOption1 = 0,
    .nArgOnlyLongOption2 = 0,
    .nArgOnlyShortOption = 0,
    .nArgLongAndShortOption = 0,
    .nArgWithParameter = 0,
    .pStringArg = nullptr,
    .bBoolean1 = false,
    .bBoolean2 = false,
    .bBoolean3 = false,
};

void Usage(const char *pProg)
{
    printf("Usage:\n"
           " %s [options...]\n\n"
           "Options:\n"
           " -h, --help\x1B[45Gshow this help text and exit\n"
           " -B, --background \x1B[45Grun in background\n"
           " -v                \x1B[45Gverbose level, support 1~3 level by v, vv, or vvv\n"
           "\n"
           "     --long-option1\x1B[45Glong option1\n"
           "     --long-option2\x1B[45Glong option2\n"
           " -S               \x1B[45Gshort option\n"
           " -L, --long-short-option\x1B[45Glong and short option\n"
           " -P, --arg-with-parameter x\x1B[45Garg with number parameter\n"
           " -s, --string-arg x\x1B[45Garg with string parameter\n"
           "     --boolean1\x1B[45Gboolean value\n"
           "     --boolean2\x1B[45Gboolean value\n"
           "     --boolean3\x1B[45Gboolean value\n"
           "\n",
           basename(pProg)
    );
}

int ParseCmdOptions(int argc, char *argv[])
{
    static struct option long_options[] =
                             {
                                 {"background",         no_argument,       nullptr,                           'B'},
                                 {"help",               no_argument,       nullptr,                           'h'},
                                 {"long-option1",       no_argument,       &G_AppOptions.nArgOnlyLongOption1, 1},
                                 {"long-option2",       required_argument, nullptr,                           0},
                                 {"long-short-option",  no_argument,       nullptr,                           'L'},
                                 {"arg-with-parameter", required_argument, nullptr,                           'P'},
                                 {"string-arg",         required_argument, nullptr,                           's'},

                                 // 不能使用bool类型的数据，如果使用bool来定义参数，则如果输入的参数顺序和这里的定义顺序不一样，则可能读取不到入参
                                 {"boolean1",           no_argument,       &G_AppOptions.bBoolean1,           1},
                                 {"boolean2",           no_argument,       &G_AppOptions.bBoolean2,           1},
                                 {"boolean3",           no_argument,       &G_AppOptions.bBoolean3,           1},

                                 // 当某些参数只支持短参数时，需要把这些参数项放到列表的末尾
                                 {nullptr,              no_argument,       nullptr,                           'S'},
                                 {nullptr,              no_argument,       nullptr,                           'v'},

                                 {nullptr, 0,                              nullptr,                           0}
                             };

    opterr = 0;

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "BhvSLP:s:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case '\x00':
                if (long_options[option_index].flag != nullptr)
                    break;
                if (strcmp(long_options[option_index].name, "long-option2") == 0)
                {
                    printf("parsed long-option2\n\n");
                }
                break;

            case 'h':
                Usage(argv[0]);
                exit(0);

            case 'B':
                G_AppOptions.nBackground = 1;
                break;

            case 'v':
                G_AppOptions.nVerbose++;
                break;

            case 'S':
                G_AppOptions.nArgOnlyShortOption = 1;
                break;

            case 'L':
                G_AppOptions.nArgLongAndShortOption = 1;
                break;

            case 'P':
                G_AppOptions.nArgWithParameter = atoi(optarg);
                break;

            case 's':
                G_AppOptions.pStringArg = optarg;
                break;

            case '?':
                switch (optopt)
                {
                    case 'P':
                    case 's':
                        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                        exit(1);
                    default:
                        if (isprint(optopt))
                            fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                        else
                            fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                        exit(1);
                }

            default:
                Usage(argv[0]);
                exit(1);
        }
    }

    return 0;
}
