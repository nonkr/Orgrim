/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/1 13:59
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <memory.h>
#include <assert.h>
#include <math.h>

int iFirstDisplay = 1;

void progress_init()
{
    iFirstDisplay = 1;
}

void display_progress(int ws_col, const char *pLabel, const int total, int current)
{
    int iLabelLen   = 0;
    int iPercent    = 0;
    int i;
    int iEqualLen   = 0;
    int iPercentLen = 0;
    int iLeftLen    = 0;
    int iSpaceLen   = 0;
    int iGreatLen   = 0;

    assert(total > 0);
    assert(current >= 0);

    if (pLabel)
    {
        iLabelLen = strlen(pLabel) + 1;
    }

    iPercent = (int) floor((double) current / total * 100);
    if (iPercent < 10)
    {
        iPercentLen = 4;
    }
    else if (iPercent < 100)
    {
        iPercentLen = 5;
    }
    else
    {
        iPercentLen = 6;
    }

    if (iFirstDisplay)
    {
        iFirstDisplay = 0;
    }
    else
    {
        for (i = 0; i < ws_col; i++)
        {
            printf("\b");
        }
    }

    if (pLabel)
    {
        printf("%s:", pLabel);
    }

    iEqualLen = (int) floor((ws_col - iLabelLen - iPercentLen) * (double) current / total);

    for (i = 0; i < iEqualLen; i++)
    {
        printf("=");
    }

    iLeftLen  = ws_col - iLabelLen - iPercentLen - iEqualLen;
    iGreatLen = (iLeftLen >= 3) ? 3 : iLeftLen;

    for (i = 0; i < iGreatLen; i++)
    {
        printf(">");
    }

    iSpaceLen = ws_col - iLabelLen - iPercentLen - iEqualLen - iGreatLen;

    for (i = 0; i < iSpaceLen; i++)
    {
        printf(" ");
    }

    printf("[%d%%]\n", iPercent);
    fflush(stdout);
}

int main()
{
    int i = 0;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    progress_init();
    for (i = 0; i <= 123; i++)
    {
        display_progress(w.ws_col, "Label", 123, i);
        usleep(50000);
    }

    return 0;
}
