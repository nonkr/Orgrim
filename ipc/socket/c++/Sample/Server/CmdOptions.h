#pragma once

#include <cstdint>

struct AppOptions_t
{
    int     bBackground = 0;
    uint8_t nVerbose    = 0;
};

extern AppOptions_t G_AppOptions;

int ParseCmdOptions(int argc, char *argv[]);
