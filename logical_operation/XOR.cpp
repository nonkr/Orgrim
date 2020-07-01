#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>

static int ConvertHexStringToBuffer(char *pHexstring, unsigned char **ppOut, int *pIOutLen)
{
    int           i;
    int           iStrLen;
    unsigned char *pOut   = nullptr;
    int           iOutLen = 0;
    int           iOffset = 0;
    unsigned int  uiTmp   = 0;

    if (pHexstring == nullptr)
        return -1;

    iStrLen = strlen(pHexstring);

    // 先初步对传入的字符串做一遍检查
    if (iStrLen == 2)
        iOutLen = 1;
    else if ((iStrLen - 2) % 3 == 0)
        iOutLen = (iStrLen - 2) / 3 + 1;
    else
    {
        fprintf(stderr, "malformed input hex string\n");
        goto error;
    }

    pOut = new unsigned char[iOutLen];

    for (i = 0; i < iStrLen;)
    {
        if (isxdigit(pHexstring[i]) == 0 || isxdigit(pHexstring[i + 1]) == 0)
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        sscanf(pHexstring + i, "%2x", &uiTmp);
        pOut[iOffset++] = (char) uiTmp;

        if (i + 2 == iStrLen)
            break;

        if (pHexstring[i + 2] != ' ')
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        i += 3;
    }

    *ppOut    = pOut;
    *pIOutLen = iOutLen;

    return 0;
error:
    delete[] pOut;
    return -1;
}

int main(int argc, char *argv[])
{
    unsigned char *pBinary   = nullptr;
    int           iBinaryLen = 0;
    int           xorValue   = 0;

    if (argc < 2)
    {
        printf("Usage: %s \"00 01 02\"\n", argv[0]);
        return 1;
    }

    printf("[%s]\n", argv[1]);
    if (ConvertHexStringToBuffer(argv[1], &pBinary, &iBinaryLen))
    {
        fprintf(stderr, "Convert send hex string failed\n");
        return 1;
    }
    printf("%d\n", iBinaryLen);
    for (int i = 0; i < iBinaryLen; i++)
    {
        xorValue ^= pBinary[i];
    }
    printf("xor:%02X\n", xorValue);
    return 0;
}
