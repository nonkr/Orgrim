/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/11 20:16
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAP_PIXELS 128

int draw_map(char (*map_data)[MAP_PIXELS])
{
//    printf("%zu %zu\n", sizeof(*map_data), sizeof(map_data[0]));

    int i, j;
    for (i = 0; i < MAP_PIXELS; i++)
    {
        for (j = 0; j < MAP_PIXELS; j++)
        {
            printf("%c", map_data[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int main(int argc, char *argv[])
{
    static char map_data[MAP_PIXELS][MAP_PIXELS];
    FILE *fp = NULL;

    fp = fopen("sizechart_1.txt", "r");
    int c;
    int row_count = 0;
    int col_count = 0;
    int k;

    while (!feof(fp))
    {
        c = fgetc(fp);
        if (c == '\n')
        {
            for (k = col_count; k <MAP_PIXELS; k++)
            {
                map_data[row_count][k] = ' ';
            }

            if (row_count >= MAP_PIXELS)
            {
                fprintf(stderr, "malformed size chart of row %d, col %d\n", row_count, col_count);
                goto error;
            }
            row_count++;
            col_count = 0;
        }
        else
        {
            map_data[row_count][col_count] = (char) c;
            if (col_count >= MAP_PIXELS)
            {
                fprintf(stderr, "malformed size chart of col %d, row %d\n", col_count, row_count);
                goto error;
            }
            col_count++;
        }
    }

    fclose(fp);

    while (1)
    {
        draw_map(map_data);
        printf("\033[1A");
        printf("\033[K");
        sleep(1);
    }

    return 0;
error:
    fclose(fp);
    return 2;
}
