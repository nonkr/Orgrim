/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/11/9 16:37
 *
 */

#include <cstdio>
#include "CmdOptions.h"

int main(int argc, char **argv)
{
    ParseCmdOptions(argc, argv);

    printf("nBackground:%d\n", G_AppOptions.nBackground);
    printf("nVerbose:%d\n", G_AppOptions.nVerbose);
    printf("nArgOnlyLongOption1:%d\n", G_AppOptions.nArgOnlyLongOption1);
    printf("nArgOnlyLongOption2:%d\n", G_AppOptions.nArgOnlyLongOption2);
    printf("nArgOnlyShortOption:%d\n", G_AppOptions.nArgOnlyShortOption);
    printf("nArgLongAndShortOption:%d\n", G_AppOptions.nArgLongAndShortOption);
    printf("nArgWithParameter:%d\n", G_AppOptions.nArgWithParameter);
    printf("pStringArg:%s\n", G_AppOptions.pStringArg);
    printf("bBoolean1:%s\n", G_AppOptions.bBoolean1 ? "true" : "false");
    printf("bBoolean2:%s\n", G_AppOptions.bBoolean2 ? "true" : "false");
    printf("bBoolean3:%s\n", G_AppOptions.bBoolean3 ? "true" : "false");

    return 0;
}
