#ifndef _DEBUGUTIL_H_
#define _DEBUGUTIL_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdlib>
#include "PrintUtil.h"

static const int PROTO_CMD_OFFSET = 3;

class DebugUtil
{
public:
    static void _mkdir(const char *dir)
    {
        char   tmp[256];
        char   *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp), "%s", dir);
        len = strlen(tmp);
        if (tmp[len - 1] == '/')
            tmp[len - 1] = 0;
        for (p = tmp + 1; *p; p++)
            if (*p == '/')
            {
                *p = 0;
                mkdir(tmp, S_IRWXU);
                *p = '/';
            }
        mkdir(tmp, S_IRWXU);
    }

    static int MkdirIfNotExists(const char *path)
    {
        struct stat sb;
        if (stat    (path, &sb) == 0 && S_ISDIR(sb.st_mode))
        {
        }
        else
        {
            _mkdir(path);
        }

        return 0;
    }

    static int GetLastFileRecord(const char *pPath, const char *pName, const char *pExtension)
    {
        struct dirent **namelist;
        int           n;
        int           iLast = -1;
        char          arrFileName[64];
        sprintf(arrFileName, "%s_%%d.%s", pName, pExtension);

        MkdirIfNotExists(pPath);

        n = scandir(pPath, &namelist, 0, versionsort);
        if (n < 0)
        {
            perror("scandir");
            return -1;
        }
        else if (n == 0)
        {
            return 0;
        }
        else
        {
            while (n--)
            {
                if (!strcmp(namelist[n]->d_name, ".") || !strcmp(namelist[n]->d_name, ".."))
                {
                    free(namelist[n]);
                    continue;
                }
                if (iLast == -1)
                {
                    if (sscanf(namelist[n]->d_name, arrFileName, &iLast) != 1)
                    {
                        iLast = -1;
                    }
                    else
                    {
                        iLast++;
                    }
                }
                free(namelist[n]);
            }
            free(namelist);

            if (iLast == -1)
            {
                return 0;
            }
            else
            {
                return iLast;
            }
        }
    }

    static FILE *OpenRecordFile(const char *pPath, const char *pName, const char *pExtension)
    {
        char arrFileName[BUFSIZ];
        FILE *fp;
        sprintf(arrFileName, "%s/%s_%d.%s", pPath, pName, GetLastFileRecord(pPath, pName, pExtension), pExtension);

        if ((fp = fopen(arrFileName, "w")) == nullptr)
        {
            FR_PRINT_RED("Failed to open %s\n", arrFileName);
        }
        else
        {
            FR_PRINT_GREEN("Open %s for %s recording\n", arrFileName, pName);
        }
        return fp;
    }

    static FILE *OpenRecordFile(const char *pPath, const char *pName)
    {
        return OpenRecordFile(pPath, pName, "log");
    }

    static void CloseRecordFile(FILE *fp)
    {
        if (fp)
        {
            fclose(fp);
        }
    }

    static int
    WriteRecordBinaryData(const char *pPath, const char *pName, const char *pExtension, const char *pData, size_t nSize)
    {
        FILE *fp = OpenRecordFile(pPath, pName, pExtension);
        if (fp == nullptr)
        {
            return 1;
        }

        fwrite(pData, nSize, 1, fp);
        CloseRecordFile(fp);

        return 0;
    }

    static int WriteRecordBinaryData(const char *pPath, const char *pName, const char *pData, size_t nSize)
    {
        return WriteRecordBinaryData(pPath, pName, "bin", pData, nSize);
    }

    /********************* overwrite series start *********************/

    static FILE *OpenRecordFileOW(const char *pPath, const char *pName, const int iIndex, const char *pExtension)
    {
        char arrFileName[BUFSIZ];
        FILE *fp;
        sprintf(arrFileName, "%s/%s_%d.%s", pPath, pName, iIndex, pExtension);

        MkdirIfNotExists(pPath);

        if ((fp = fopen(arrFileName, "w")) == nullptr)
        {
            FR_PRINT_RED("Failed to open %s\n", arrFileName);
        }
//        else
//        {
//            FR_PRINT_GREEN("Open %s for %s recording\n", arrFileName, pName);
//        }
        return fp;
    }

    static void CloseRecordFileOW(FILE *fp)
    {
        if (fp)
        {
            fclose(fp);
        }
    }

    static int WriteRecordBinaryDataOW(const char *pPath, const char *pName, const int iIndex, const char *pExtension,
                                       const char *pData, size_t nSize)
    {
        FILE *fp = OpenRecordFileOW(pPath, pName, iIndex, pExtension);
        if (fp == nullptr)
        {
            return 1;
        }

        fwrite(pData, nSize, 1, fp);
        CloseRecordFileOW(fp);

        return 0;
    }

    static int WriteRecordBinaryDataOW(const char *pPath, const char *pName, const int iIndex, const char *pData,
                                       size_t nSize)
    {
        return WriteRecordBinaryDataOW(pPath, pName, iIndex, "bin", pData, nSize);
    }

    /********************* overwrite series end *********************/

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

    static void BinToHexString(const unsigned char *pData, int iDataLen, unsigned char **ppOutData, int *pOutDataLen)
    {
        if (iDataLen <= 0)
        {
            *ppOutData   = nullptr;
            *pOutDataLen = 0;
            return;
        }

        *pOutDataLen = (iDataLen * 3 - 1);

        unsigned char *pOutData = new unsigned char[*pOutDataLen];
        int           iOffset   = 0;

        for (int i = 0; i < iDataLen; i++)
        {
            if (iOffset > 0)
            {
                snprintf((char *) pOutData + iOffset, 2, " ");
                iOffset++;
            }
            snprintf((char *) pOutData + iOffset, 3, "%02X", pData[i]);
            iOffset += 2;
        }

        *ppOutData = pOutData;
    }

    static void PrintAsHexString(const char *pLabel, const unsigned char *pData, int iDataLen)
    {
        unsigned char cChecksum     = 0;
        char          arrBuff[1024] = {0};
        int           iBuffOffset   = 0;
        int           iStrLen       = 0;

        if (pData[3] == 0x85)
        {
            if (pData[4] == 0x01 && pData[5] == 0x86)
            {
            }
            else
            {
                return;
            }
        }

        iStrLen = strlen(pLabel) + 3;
        snprintf(arrBuff + iBuffOffset, iStrLen, "%s:[", pLabel);
        iBuffOffset += iStrLen - 1;

        for (int i = 0; i < iDataLen; i++)
        {
            if (i >= 3 && i < iDataLen - 1)
            {
                cChecksum += pData[i];
            }

            if (i == iDataLen - 1)
            {
                if ((cChecksum & 0xFF) != (pData[i] & 0xFF))
                {
                    iStrLen = strlen(KRED) + 2 + strlen(KRESET) + 1;
                    snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s", KRED, pData[i], KRESET);
                    iBuffOffset += iStrLen - 1;
                }
                else
                {
                    iStrLen = strlen(KYELLOW) + 2 + strlen(KRESET) + 1;
                    snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s", KYELLOW, pData[i], KRESET);
                    iBuffOffset += iStrLen - 1;
                }
            }
            else if (i == 0)
            {
                iStrLen = strlen(KLIMEGREEN) + 3 + strlen(KRESET) + 1;
                snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KLIMEGREEN, pData[i], KRESET);
                iBuffOffset += iStrLen - 1;
            }
            else if (i == 1 || i == 2)
            {
                iStrLen = strlen(KORANGE) + 3 + strlen(KRESET) + 1;
                snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KORANGE, pData[i], KRESET);
                iBuffOffset += iStrLen - 1;
            }
            else if (i == 3)
            {
                iStrLen = strlen(KBLUE) + 3 + strlen(KRESET) + 1;
                snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KBLUE, pData[i], KRESET);
                iBuffOffset += iStrLen - 1;
            }
            else if (i == 4)
            {
                switch (pData[PROTO_CMD_OFFSET])
                {
                    case 0x03:
                        iStrLen = strlen(KCYAN) + 3 + strlen(KRESET) + 1;
                        snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KCYAN, pData[i], KRESET);
                        iBuffOffset += iStrLen - 1;
                        break;
                    default:
                        iStrLen = 4;
                        snprintf(arrBuff + iBuffOffset, iStrLen, "%02X ", pData[i]);
                        iBuffOffset += iStrLen - 1;
                }
            }
            else
            {
                iStrLen = 4;
                snprintf(arrBuff + iBuffOffset, iStrLen, "%02X ", pData[i]);
                iBuffOffset += iStrLen - 1;
            }
        }

        iStrLen = strlen("]\n") + 1;
        snprintf(arrBuff + iBuffOffset, iStrLen, "]\n");

        FR_PRINT("%s", arrBuff);
    }
};

#endif //_DEBUGUTIL_H_
