#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <ctime>
#include "CmdOptions.h"
#include "version.h"

AppOptions_t G_AppOptions;

void Usage(const char *pProg)
{
    printf("Usage:\n"
           " %s [options...]\n\n"
           "Options:\n"
           " -h, --help\x1B[45Gshow this help text and exit\n"
           " -B, --background \x1B[45Grun in background\n"
           " -V, --version\x1B[45Gshow this application's build version\n"
           "     --full-version\x1B[45Gshow this application's full build version\n"
           " -v                \x1B[45Gverbose level, support 1~3 level by v, vv, or vvv\n"
           "\n"
           " -I, --ip <ip>\x1B[45Gserver ip\n"
           "\n",
           basename(pProg)
    );
}

char *GetAppVersion()
{
    static char tmBuffer[16] = {0};
    sprintf(tmBuffer, "V%d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);
    return tmBuffer;
}

char *GetAppStandardVersion()
{
    char        tmBuffer[16]  = {0};
    static char tmBuffer2[32] = {0};
    struct tm   tmTemp;

    strptime(APP_VERSION_BUILD_DATE, "%b %d %Y", &tmTemp);
    strftime(tmBuffer, 16, "build %y%m%d", &tmTemp);

    sprintf(tmBuffer2, "%s %s", GetAppVersion(), tmBuffer);

    return tmBuffer2;
}

char *GetAppFullVersion()
{
    char        tmBuffer[8]   = {0};
    static char tmBuffer2[64] = {0};
    struct tm   tmTemp;

    strptime(APP_VERSION_BUILD_TIME, "%T", &tmTemp);
    strftime(tmBuffer, 8, "%H%M%S", &tmTemp);

    sprintf(tmBuffer2, "%s-%s", GetAppStandardVersion(), tmBuffer);

    return tmBuffer2;
}

int ParseCmdOptions(int argc, char *argv[])
{
    int                  c;
    static struct option long_options[] =
                             {
                                 {"background",   no_argument,       nullptr, 'B'},
                                 {"help",         no_argument,       nullptr, 'h'},
                                 {"version",      no_argument,       nullptr, 'V'},
                                 {"full-version", no_argument,       nullptr, 0},

                                 {"ip",           required_argument, nullptr, 'I'},

                                 {nullptr,        no_argument,       nullptr, 'v'},
                                 {nullptr, 0,                        nullptr, 0}
                             };

    opterr = 0;
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "BhVvI:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case '\x00':
                if (long_options[option_index].flag != nullptr)
                    break;
                if (strcmp(long_options[option_index].name, "full-version") == 0)
                {
                    printf("%s %s\n", basename(argv[0]), GetAppFullVersion());
                    exit(0);
                }
                break;

            case 'h':
                Usage(argv[0]);
                exit(0);

            case 'B':
                G_AppOptions.bBackground = 1;
                break;

            case 'v':
                G_AppOptions.nVerbose++;
                break;

            case 'V':
                printf("%s %s\n", basename(argv[0]), GetAppStandardVersion());
                exit(0);

            case 'I':
                G_AppOptions.pServerIP = optarg;
                break;

            case '?':
                switch (optopt)
                {
                    case 'I':
                        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                        Usage(argv[0]);
                        exit(1);
                    default:
                        if (isprint(optopt))
                            fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                        else
                            fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                        Usage(argv[0]);
                        exit(1);
                }

            default:
                Usage(argv[0]);
                exit(1);
        }
    }

    return 0;
}
