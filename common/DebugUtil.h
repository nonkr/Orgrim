#ifndef _DEBUGUTIL_H_
#define _DEBUGUTIL_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dirent.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "log/log.h"
#include "PrintUtil.h"
#include "SysUtil.h"
//#include "CmdOptions.h"

class DebugUtil
{
public:

    static int GetLastFileRecord(const char *pPath, const char *pName, const char *pExtension)
    {
        struct dirent **namelist;
        int           n;
        int           iLast = -1;
        char          arrFileName[64];
        sprintf(arrFileName, "%s_%%d.%s", pName, pExtension);

        SysUtil::MkdirIfNotExists(pPath);

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
                    char *pos = strchr(namelist[n]->d_name, '.');
                    if (pos)
                    {
                        if (strlen(namelist[n]->d_name) - (pos - namelist[n]->d_name) - 1 == strlen(pExtension))
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

    static int GetLastRecordDir(const char *pPath)
    {
        struct dirent **namelist;
        int           n;
        int           iLast           = -1;
        char          arrFileName[32] = {0};
        sprintf(arrFileName, "%%d");

        SysUtil::MkdirIfNotExists(pPath);

        n = scandir(pPath, &namelist, 0, versionsort);
        if (n < 0)
        {
            perror("scandir");
            return -1;
        }
        else if (n == 0)
        {
            return -1;
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
                if (namelist[n]->d_type == 4 && iLast == -1 && sscanf(namelist[n]->d_name, arrFileName, &iLast) != 1)
                {
                    iLast = -1;
                }
                free(namelist[n]);
            }
            free(namelist);

            if (iLast == -1)
            {
                return -1;
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
        FILE *fp = nullptr;
        sprintf(arrFileName, "%s/%s_%d.%s", pPath, pName, GetLastFileRecord(pPath, pName, pExtension), pExtension);

        if ((fp = fopen(arrFileName, "w")) == nullptr)
        {
            FR_PRINT_RED("DebugUtil", "Failed to open %s\n", arrFileName);
        }
        else
        {
            FR_PRINT_GREEN("DebugUtil", "Open %s for %s recording\n", arrFileName, pName);
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

    static FILE *OpenRecordFileOW(const char *pPath, const char *pName, const char *pExtension)
    {
        char arrFileName[BUFSIZ];
        FILE *fp;
        sprintf(arrFileName, "%s/%s.%s", pPath, pName, pExtension);

        SysUtil::MkdirIfNotExists(pPath);

        if ((fp = fopen(arrFileName, "w")) == nullptr)
        {
            FR_PRINT_RED("DebugUtil", "Failed to open %s\n", arrFileName);
        }
        return fp;
    }

    static FILE *OpenRecordFileOW(const char *pPath, const char *pName, const int iIndex, const char *pExtension)
    {
        char arrFileName[BUFSIZ];
        FILE *fp;

        if (iIndex >= 0)
        {
            sprintf(arrFileName, "%s/%s_%d.%s", pPath, pName, iIndex, pExtension);
        }
        else
        {
            sprintf(arrFileName, "%s/%s.%s", pPath, pName, pExtension);
        }

        SysUtil::MkdirIfNotExists(pPath);

        if ((fp = fopen(arrFileName, "w")) == nullptr)
        {
            FR_PRINT_RED("DebugUtil", "Failed to open %s\n", arrFileName);
        }
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

    static int WriteRecordBinaryDataOW(const char *pPath, const char *pName, const char *pData, size_t nSize)
    {
        return WriteRecordBinaryDataOW(pPath, pName, -1, "bin", pData, nSize);
    }

    /********************* overwrite series end *********************/

    /********************* append series start *********************/

    static FILE *OpenAppendFile(const char *pPath, const char *pName)
    {
        char arrFileName[BUFSIZ];
        FILE *fp;
        sprintf(arrFileName, "%s/%s.bin", pPath, pName);

        unlink(arrFileName);

        SysUtil::MkdirIfNotExists(pPath);

        if ((fp = fopen(arrFileName, "a+")) == nullptr)
        {
            FR_PRINT_RED("DebugUtil", "Failed to open %s\n", arrFileName);
        }
        return fp;
    }

    static long long AppendBinaryData(FILE *fp, const char *pData, size_t nSize)
    {
        if (fp == nullptr)
        {
            return 0;
        }

        if (fwrite(pData, nSize, 1, fp))
        {
            return nSize;
        }

        return 0;
    }

    /********************* append series end *********************/

    static int BinaryCompare(const unsigned char *pBin1, size_t sLen1, const unsigned char *pBin2, size_t sLen2)
    {
        unsigned char digest1[16];
        unsigned char digest2[16];

        MD5(pBin1, sLen1, digest1);
        MD5(pBin2, sLen2, digest2);

//        char md5string1[33];
//        char md5string2[33];
//        for (int i = 0; i < 16; ++i)
//        {
//            sprintf(&md5string1[i * 2], "%02x", (unsigned int) digest1[i]);
//            sprintf(&md5string2[i * 2], "%02x", (unsigned int) digest2[i]);
//        }
//        printf("%s\n", md5string1);
//        printf("%s\n", md5string2);

        return memcmp(digest1, digest2, sizeof(digest1));
    }

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

    static void PrintAsHexString(const char *pLabel, const unsigned char *pData, int iDataLen, bool bCheckIgnore)
    {
        unsigned char cChecksum     = 0;
        char          arrBuff[4096] = {0};
        int           iBuffOffset   = 0;
        int           iStrLen       = 0;
        int           iSkip         = 0;
        int           iOneLen       = 0;

//        FR_PRINT_ORANGE("DebugUtil", "PrintAsHexString() enter\n");
//        if (strncmp(pLabel, "RecvFailed", 10) == 0)
//        {
//            printf(KRED "%s:[" KRESET, pLabel);
//        }
//        else
//        {
//            printf(KGREEN "%s:[" KRESET, pLabel);
//        }
//        for (int i = 0; i < iDataLen; i++)
//        {
////            printf("0x%02X ", pData[i] & 0xFF);
//            printf("%d ", pData[i] & 0xFF);
//        }
//        printf("]\n");
//        return;

        if (iDataLen > 1024)
        {
            FR_ALOG_RED("DebugUtil", "PrintAsHexString() %s: too long data size:%d\n", pLabel, iDataLen);
            return;
        }

        while (pData[1 + iSkip] + (pData[2 + iSkip] << 8) + 4 + iSkip <= iDataLen)
        {
            if (pData[0 + iSkip] != 0xAA)
            {
                return;
            }

#if 0
            if (bCheckIgnore)
            {
                if (pData[3 + iSkip] == 0x81)
                {
                    // ignore imu
                    return;
                }
                else if (pData[3 + iSkip] == 0x85)
                {
                    if (pData[4 + iSkip] == 0x01 && pData[5 + iSkip] == 0x86)
                    {
                        if (strncmp(pLabel, "Send", 4) == 0)
                        {
                            return;
                        }
                    }
                    else
                    {
                        return;
                    }
                }
                else if (pData[3 + iSkip] == 0x06)
                {
                    if ((g_stRobotOption.stAppDebugSwitch.iUARTPrintSendIgnoreCheckStatus &&
                         strncmp(pLabel, "Send", 4) == 0) ||
                        (g_stRobotOption.stAppDebugSwitch.iUARTPrintRecvIgnoreCheckStatus &&
                         strncmp(pLabel, "Recv", 4) == 0))
                    {
                        return;
                    }
                }
                else if (pData[3 + iSkip] == 0x08)
                {
                    if (g_stRobotOption.stAppDebugSwitch.iUARTPrintSendIgnoreSendObstacle &&
                        strncmp(pLabel, "Send", 4) == 0)
                    {
                        return;
                    }
                }
                else if (pData[3 + iSkip] == 0x03)
                {
                    if (pData[4 + iSkip] == 0x05)
                    {
                        if (g_stRobotOption.stAppDebugSwitch.iUARTPrintSendIgnoreDownloadMap &&
                            strncmp(pLabel, "Send", 4) == 0)
                        {
                            return;
                        }
                    }
                }
                else if (pData[3 + iSkip] == 0x90)
                {
                    if ((g_stRobotOption.stAppDebugSwitch.iUARTPrintSendIgnoreHeartbeat &&
                         strncmp(pLabel, "Send", 4) == 0) ||
                        (g_stRobotOption.stAppDebugSwitch.iUARTPrintRecvIgnoreHeartbeat &&
                         strncmp(pLabel, "Recv", 4) == 0))
                    {
                        return;
                    }
                }
            }
#endif

            iOneLen   = pData[1 + iSkip] + (pData[2 + iSkip] << 8) + 4;
            cChecksum = 0;

            iStrLen = strlen(pLabel) + 3;
            snprintf(arrBuff + iBuffOffset, iStrLen, "%s:[", pLabel);
            iBuffOffset += iStrLen - 1;

            for (int i = 0; i < iOneLen; i++)
            {
                if (i >= 3 && i < iOneLen - 1)
                {
                    cChecksum += pData[i + iSkip];
                }

                if (i == iOneLen - 1)
                {
                    if ((cChecksum & 0xFF) != (pData[i + iSkip] & 0xFF))
                    {
                        iStrLen = strlen(KRED) + 2 + strlen(KRESET) + 1;
                        snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s", KRED, pData[i + iSkip], KRESET);
                        iBuffOffset += iStrLen - 1;
                    }
                    else
                    {
                        iStrLen = strlen(KYELLOW) + 2 + strlen(KRESET) + 1;
                        snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s", KYELLOW, pData[i + iSkip], KRESET);
                        iBuffOffset += iStrLen - 1;
                    }
                }
                else if (i == 0)
                {
                    iStrLen = strlen(KLIMEGREEN) + 3 + strlen(KRESET) + 1;
                    snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KLIMEGREEN, pData[i + iSkip], KRESET);
                    iBuffOffset += iStrLen - 1;
                }
                else if (i == 1 || i == 2)
                {
                    iStrLen = strlen(KORANGE) + 3 + strlen(KRESET) + 1;
                    snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KORANGE, pData[i + iSkip], KRESET);
                    iBuffOffset += iStrLen - 1;
                }
                else if (i == 3)
                {
                    iStrLen = strlen(KBLUE) + 3 + strlen(KRESET) + 1;
                    snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KBLUE, pData[i + iSkip], KRESET);
                    iBuffOffset += iStrLen - 1;
                }
                else if (i == 4)
                {
                    switch (pData[3])
                    {
                        case 0x03:
                            iStrLen = strlen(KCYAN) + 3 + strlen(KRESET) + 1;
                            snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X%s ", KCYAN, pData[i + iSkip], KRESET);
                            iBuffOffset += iStrLen - 1;
                            break;
                        default:
                            iStrLen = 4;
                            snprintf(arrBuff + iBuffOffset, iStrLen, "%02X ", pData[i + iSkip]);
                            iBuffOffset += iStrLen - 1;
                    }
                }
                else
                {
                    iStrLen = 4;
                    snprintf(arrBuff + iBuffOffset, iStrLen, "%02X ", pData[i + iSkip]);
                    iBuffOffset += iStrLen - 1;
                }
            }

            iStrLen = strlen("]\n") + 1;
            snprintf(arrBuff + iBuffOffset, iStrLen, "]\n");
            iBuffOffset += iStrLen - 1;

            if (pData[1 + iSkip] + (pData[2 + iSkip] << 8) + 4 + iSkip == iDataLen)
            {
                break;
            }

            iSkip += pData[1 + iSkip] + (pData[2 + iSkip] << 8) + 4;
        }

        if (iBuffOffset > 0) FR_ALOG("DebugUtil", "%s", arrBuff);
    }

    static uint32_t GetSystemFreeMemory()
    {
        struct sysinfo s_info;
        int            error = sysinfo(&s_info);
        return s_info.freeram;
    }
};

#endif //_DEBUGUTIL_H_
