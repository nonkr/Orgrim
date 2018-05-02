/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/5/2 16:41
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
    DIR           *FD;
    struct dirent *in_file;

    /* Scanning the in directory */
    if (NULL == (FD = opendir(argv[1])))
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return 1;
    }
    printf("get filenames under [%s]\n", argv[1]);

    while ((in_file = readdir(FD)))
    {
        /* On linux/Unix we don't want current and parent directories
         * On windows machine too, thanks Greg Hewgill
         */
        if (!strcmp(in_file->d_name, ".") || !strcmp(in_file->d_name, ".."))
        {
            continue;
        }

        if (in_file->d_type == DT_REG)
        {
            printf("[%s]\n", in_file->d_name);
        }
    }

    closedir(FD);

    return 0;
}
