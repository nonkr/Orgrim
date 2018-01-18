/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/4 23:54
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory.h>

extern HIST_ENTRY **history_list();

int main()
{
    char *temp = NULL;
    int done = 0;

    while (!done)
    {
        temp = readline("readline$ ");

        /* Test for EOF. */
        if (!temp)
        {
            exit(1);
        }

        /* If there is anything on the line, print it and remember it. */
        if (*temp)
        {
            fprintf(stderr, "%s\n", temp);
            add_history(temp);
        }

        /* Check for `command' that we handle. */
        if (strcmp(temp, "quit") == 0)
        {
            done = 1;
        }
        else if (strcmp(temp, "list") == 0)
        {
            HIST_ENTRY **list;
            register int i;
            list = history_list();
            if (list)
            {
                for (i = 0; list[i]; i++)
                {
                    fprintf(stderr, "%d: %s\n", i, list[i]->line);
                }
            }
        }

        free(temp);
    }

    exit(0);
}
