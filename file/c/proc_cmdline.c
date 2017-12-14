/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

int main(int argc, char **argv)
{
    FILE   *cmdline = fopen("/proc/self/cmdline", "rb");
    char   *arg     = NULL;
    size_t size     = 0;
    char cmdstr[ARG_MAX] = {0};

    while (getdelim(&arg, &size, 0, cmdline) != -1)
    {
        puts(arg);
        strcat(cmdstr, arg);
        strcat(cmdstr, " ");
    }
    cmdstr[strlen(cmdstr) - 1] = '\0';
    free(arg);
    fclose(cmdline);

    printf("cmdline: [%s]\n", cmdstr);

    return EXIT_SUCCESS;
}