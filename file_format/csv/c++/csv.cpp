#include <cstring>
#include "csv.h"

int CSV::ParseLine(const char *pStr, vector<string> &oneline)
{
    const char *pDup       = pStr;
    const char *pPosComma  = nullptr;
    char       buffer[128] = {0};
    int        strlength   = 0;
    string     s;

    oneline.clear();

    while (true)
    {
        pPosComma = strchr(pDup, ',');
        if (pPosComma == nullptr)
        {
            if (pDup && strlen(pDup) > 0)
            {
                strlength = strlen(pDup);
                memset(buffer, 0x00, sizeof(buffer));
                memcpy(buffer, pDup, strlength);
                s = buffer;
                oneline.emplace_back(s);
            }
            break;
        }

        strlength = pPosComma - pDup;

        memset(buffer, 0x00, sizeof(buffer));
        memcpy(buffer, pDup, strlength);
        s = buffer;
        oneline.emplace_back(s);
        pDup = pDup + (strlength + 1);
    }

    return 0;
}
