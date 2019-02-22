/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/2/22 14:22
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    char    *file = argv[1];
    FILE    *stream;
    char    *line = NULL;
    size_t  len   = 0;
    ssize_t read;

    stream = fopen(file, "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1)
    {
//        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
    }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}
