/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/9 10:12
 *
 */

#ifndef _PROGRESS_H_
#define _PROGRESS_H_

#include <cmath>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <sys/ioctl.h>

class Progress
{
public:
    Progress()
    {
        struct winsize w = {0};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        m_iWindowColumns = w.ws_col;
        m_iFirstDisplay  = true;
    }

    void Reset()
    {
        struct winsize w = {0};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        m_iWindowColumns = w.ws_col;
        m_iFirstDisplay = true;
    }

    void Display(const char *pLabel, const int total, int current)
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

        if (m_iFirstDisplay)
        {
            m_iFirstDisplay = false;
        }
        else
        {
            for (i = 0; i < m_iWindowColumns; i++)
            {
                printf("\b");
            }
        }

        if (pLabel)
        {
            printf("%s:", pLabel);
        }

        iEqualLen = (int) floor((m_iWindowColumns - iLabelLen - iPercentLen) * (double) current / total);

        for (i = 0; i < iEqualLen; i++)
        {
            printf("=");
        }

        iLeftLen  = m_iWindowColumns - iLabelLen - iPercentLen - iEqualLen;
        iGreatLen = (iLeftLen >= 3) ? 3 : iLeftLen;

        for (i = 0; i < iGreatLen; i++)
        {
            printf(">");
        }

        iSpaceLen = m_iWindowColumns - iLabelLen - iPercentLen - iEqualLen - iGreatLen;

        for (i = 0; i < iSpaceLen; i++)
        {
            printf(" ");
        }

        printf("[%d%%]\n", iPercent);
        fflush(stdout);
    }

private:
    int  m_iWindowColumns;
    bool m_iFirstDisplay;
};

#endif //_PROGRESS_H_
