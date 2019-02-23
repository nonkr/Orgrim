/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/2/22 19:27
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    char    *file               = argv[1];
    FILE    *pTxt;
    FILE    *pBin;
    char    *line               = NULL;
    size_t  len                 = 0;
    ssize_t read;
    char    arrBinFilename[256] = {0x00};
    int     value;

    char *dot = strrchr(file, '.');
    if (dot)
    {
        memcpy(arrBinFilename, file, dot - file);
        memcpy(arrBinFilename + (dot - file), ".bin", 4);
    }
    else
    {
        sprintf(arrBinFilename, "%s.bin", file);
    }

    pTxt = fopen(file, "r");
    if (pTxt == NULL)
        exit(EXIT_FAILURE);

    pBin = fopen(arrBinFilename, "w");
    if (pBin == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, pTxt)) != -1)
    {
        if (read > 1)
        {
            value = strtol(line, NULL, 10);
            fwrite((const void *) &value, sizeof(short), 1, pBin);
        }
    }

    free(line);
    fclose(pBin);
    fclose(pTxt);
    exit(EXIT_SUCCESS);
}
