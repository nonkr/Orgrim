#ifndef _CMDOPTIONS_H_
#define _CMDOPTIONS_H_

#include <cstdint>

struct AppOptions_t
{
    int     bBackground           = 0;
    uint8_t nVerbose              = 0;
};

extern AppOptions_t G_AppOptions;

int ParseCmdOptions(int argc, char *argv[]);

#endif //_CMDOPTIONS_H_
