/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/28 20:20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <math.h>
#include <errno.h>
#include "../../color.h"
#include "serial.h"

#define UART_BUFF_MEX_LEN 512
static const int G_MAGIC_NUMBER = 0xAA;
//static const int MAX_LEN      = UART_BUFF_MEX_LEN;  //缓冲区最大长度

int g_nUsartfd;

#define MAP_PIXELS 128
#define MAPDATA_FRAME_MAX_BYTES 200
#define UART_MAGIC_SIZE 1
#define UART_LENGTH_SIZE 2
#define UART_CMD_SIZE 1
#define UART_SUBCMD_SIZE 1
#define UART_CHECKSUM_SIZE 1

#define CRC_TABLE_SIZE 256
#define CRC_GEN_POLY   0x8005

static unsigned char g_Mapdata[MAP_PIXELS * MAP_PIXELS / 2];
static unsigned int  g_uiMapDataSendOffset;

pthread_mutex_t g_ReadMutex;
pthread_cond_t  g_ReadCond;
static char     g_ReadBuff[UART_BUFF_MEX_LEN];
static int      g_ReadBuffLen   = 0;

static unsigned long long g_ullTotalRecvCount    = 1ULL;
static unsigned long long g_ullTotalErrRecvCount = 0ULL;

static int           g_uiTotalMapDataLeft;
static unsigned char g_ucFrameNo                 = 1;
static int           g_iReplyErrorCount          = 0;

typedef enum
{
    RECV_EOK,
    RECV_EGENERIC,
    RECV_ETIMEDOUT,
}                    recv_err_t;

static unsigned int m_CRCTable[CRC_TABLE_SIZE] = {};

int hexstring_to_bytearray(char *p_hexstring, char **pp_out, int *p_i_out_len)
{
    int    i;
    size_t i_str_len;
    char   *p_out    = NULL;
    int    i_out_len = 0;
    int    i_offset  = 0;

    if (p_hexstring == NULL)
        return -1;

    i_str_len = strlen(p_hexstring);

    // 先初步对传入的字符串做一遍检查
    if (i_str_len == 2)
        i_out_len = 1;
    else if ((i_str_len - 2) % 3 == 0)
        i_out_len = (i_str_len - 2) / 3 + 1;
    else
    {
        fprintf(stderr, "malformed input hex string\n");
        goto error;
    }

    p_out = (char *) malloc((size_t) i_out_len);
    if (p_out == NULL)
        return -1;

    for (i = 0; i < i_str_len;)
    {
        if (isxdigit(p_hexstring[i]) == 0 || isxdigit(p_hexstring[i + 1]) == 0)
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        sscanf(p_hexstring + i, "%2x", (unsigned int *) &(p_out[i_offset++]));

        if (i + 2 == i_str_len)
            break;

        if (p_hexstring[i + 2] != ' ')
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        i += 3;
    }

    *pp_out      = p_out;
    *p_i_out_len = i_out_len;

    return 0;
error:
    if (p_out)
        free(p_out);
    return -1;
}

void print_as_hexstring(const int iIsSend, const char *pData, int iDataLen)
{
    unsigned char check_sum = 0;
    int           i;

    if (iIsSend)
        OGM_PRINT_GREEN("Send:[");
    else
        OGM_PRINT_ORANGE("Recv:[");

    for (i = 0; i < iDataLen; i++)
    {
        if (i >= 3 && i < iDataLen - 1)
        {
            check_sum += pData[i] & 0xFF;
        }

        if (i == iDataLen - 1)
        {
            if ((check_sum & 0xFF) != (pData[i] & 0xFF))
            {
                OGM_PRINT_RED("%02X", pData[i] & 0xFF);
            }
            else
            {
                OGM_PRINT_YELLOW("%02X", pData[i] & 0xFF);
            }
        }
        else if (i == 0)
        {
            OGM_PRINT_LIMEGREEN("%02X ", pData[i] & 0xFF);
        }
        else if (i == 1 || i == 2)
        {
            OGM_PRINT_ORANGE("%02X ", pData[i] & 0xFF);
        }
        else if (i == 3)
        {
            OGM_PRINT_BLUE("%02X ", pData[i] & 0xFF);
        }
        else if (i == 4)
        {
            switch (pData[3] & 0xFF)
            {
                case 0x03:
                    OGM_PRINT_CYAN("%02X ", pData[i] & 0xFF);
                    break;
                default:
                    OGM_PRINT("%02X ", pData[i] & 0xFF);
            }
        }
        else
        {
            OGM_PRINT("%02X ", pData[i] & 0xFF);
        }
    }

    if (iIsSend)
        OGM_PRINT_GREEN("]\n");
    else
        OGM_PRINT_ORANGE("]\n");
}

int isChecksumError(const char *pData, const int iDataLen)
{
    int           i;
    unsigned char cChecksum = 0;
    for (i = 3; i < iDataLen - 1; i++)
    {
        cChecksum += pData[i] & 0xFF;
    }

    return cChecksum == (pData[iDataLen - 1] & 0xFF) ? 0 : 1;
}

recv_err_t ReadDataFromUart()
{
    int             iRet = RECV_EOK;
    struct timespec stCondTimeout;

    clock_gettime(CLOCK_REALTIME, &stCondTimeout);
    stCondTimeout.tv_sec += 5;

    pthread_mutex_lock(&g_ReadMutex);
    while (g_ReadBuffLen <= 0)
    {
        if (pthread_cond_timedwait(&g_ReadCond, &g_ReadMutex, &stCondTimeout) == ETIMEDOUT)
        {
            OGM_PRINT_YELLOW("recv wait timed out\n");
            g_ReadBuffLen = 0;
            pthread_mutex_unlock(&g_ReadMutex);
            return RECV_ETIMEDOUT;
        }
    }

    if (g_ReadBuffLen >= 6 && g_ReadBuff[3] == 0x03)
    {
        OGM_PRINT_BLUE("Recv subcmd:[0x%02X]\n", g_ReadBuff[4]);
        if (g_ReadBuff[4] == 0x05) // A.7.4 地图下发
        {
            g_ullTotalRecvCount++;
            if (isChecksumError(g_ReadBuff, g_ReadBuffLen))
            {
                g_ullTotalErrRecvCount++;
            }
            else
            {
                if (g_ReadBuff[5] != 0x00)
                {
                    g_ullTotalErrRecvCount++;

                    g_iReplyErrorCount++;
                    OGM_PRINT_RED("Reply error, count:[%d]\n", g_iReplyErrorCount);
                    if (g_iReplyErrorCount > 3)
                    {
                        OGM_PRINT_RED("Reply error exceed the limit\n");
                        iRet = RECV_EGENERIC;
                    }
                }
            }
        }
    }

    g_ReadBuffLen = 0;
    pthread_mutex_unlock(&g_ReadMutex);

    return iRet;
}

unsigned int BlockCRC16TableFor8813(unsigned char *Block, unsigned int Len,
                                    unsigned int seed, unsigned int data_cmpl)
{
    unsigned int  CRC;
    unsigned char data;
    unsigned int  i;
    CRC    = seed;
    for (i = 0; i < Len; i++)
    {
        data = (*Block++);
        if (data_cmpl)
        {
            data = ~data;
        }
        CRC  = ((CRC << 8) & 0xFFFF) ^ (*(m_CRCTable + ((CRC >> 8) ^ data)));
    }
    return CRC;
}

unsigned int GenCRC(unsigned char *Block, unsigned int Len)
{
    return BlockCRC16TableFor8813(Block, Len, 0, 0);
}

void BuildCRC16TableFor8813(unsigned int genPoly)
{
    OGM_PRINT_BLUE("BuildCRC16TableFor8813...\n");
    unsigned int  i, j, nData;
    unsigned long crc;

    for (i = 0; i < CRC_TABLE_SIZE; i++)
    {
        nData  = i << 8;
        crc    = 0;
        for (j = 0; j < 8; j++)
        {
            if ((nData ^ crc) & 0x8000)
                crc = (crc << 1) ^ genPoly;
            else
                crc <<= 1;
            nData <<= 1;
        }
        *(m_CRCTable + i) = crc & 0xFFFF;
    }
}

unsigned char CalcChecksum(const unsigned char *pBuff, const int iBuffLen)
{
    int           i;
    unsigned char ucChecksum = 0;
    for (i = 0; i < iBuffLen; i++)
    {
        ucChecksum += pBuff[i] & 0xFF;
    }
    return ucChecksum;
}

void *SendThread(void *arg)
{
    printf("SendThread...\n");

    sleep(1);

    unsigned char ucFrameTotal   = (unsigned char) ceil(
        MAP_PIXELS * 1.0 * MAP_PIXELS / 2 / MAPDATA_FRAME_MAX_BYTES);
    unsigned char *pBuff         = NULL;
    int           iBuffLen;
    unsigned int  uiTotalMapData = MAP_PIXELS * MAP_PIXELS / 2;
    int           iFrameDataBytesLen;   // 当前帧的地图数据长度
    unsigned int  uiCRC;

    BuildCRC16TableFor8813(CRC_GEN_POLY);

    g_ucFrameNo           = 1;
    g_uiTotalMapDataLeft  = uiTotalMapData;
    g_uiMapDataSendOffset = 0;

    do
    {
        // 当前帧的地图数据长度
        iFrameDataBytesLen = (g_uiTotalMapDataLeft >= MAPDATA_FRAME_MAX_BYTES ?
                              MAPDATA_FRAME_MAX_BYTES : g_uiTotalMapDataLeft);

        // 总的包长
        iBuffLen = UART_MAGIC_SIZE + UART_LENGTH_SIZE + UART_CMD_SIZE + UART_SUBCMD_SIZE +
                   1 + 1 + 1 + iFrameDataBytesLen + 2 + UART_CHECKSUM_SIZE;

        if ((pBuff = malloc(sizeof(unsigned char) * iBuffLen)) == NULL)
        {
            OGM_PRINT_RED("malloc error\n");
            pthread_exit((void *) 1);
        }
        memset(pBuff, 0x00, sizeof(unsigned char) * iBuffLen);

        pBuff[0] = (unsigned char) G_MAGIC_NUMBER;
        pBuff[1] = (unsigned char) ((iBuffLen - UART_MAGIC_SIZE - UART_LENGTH_SIZE - UART_CHECKSUM_SIZE) >> 8);
        pBuff[2] = (unsigned char) ((iBuffLen - UART_MAGIC_SIZE - UART_LENGTH_SIZE - UART_CHECKSUM_SIZE) & 0xFF);
        pBuff[3] = (unsigned char) 0x03;
        pBuff[4] = (unsigned char) 0x05;
        pBuff[5] = ucFrameTotal; // 共有多少帧
        pBuff[6] = g_ucFrameNo;  // 当前帧号（从1开始计数）
        pBuff[7] = (unsigned char) (iFrameDataBytesLen & 0xFf);
        memcpy(pBuff + 8, g_Mapdata + g_uiMapDataSendOffset, (size_t) iFrameDataBytesLen);
        uiCRC = GenCRC(pBuff + UART_MAGIC_SIZE + UART_LENGTH_SIZE + UART_CMD_SIZE + UART_SUBCMD_SIZE,
                       (unsigned int) 1 + 1 + 1 + iFrameDataBytesLen);
        pBuff[iBuffLen - 3] = (unsigned char) (uiCRC >> 8);
        pBuff[iBuffLen - 2] = (unsigned char) (uiCRC & 0xFf);
        pBuff[iBuffLen - 1] = CalcChecksum(pBuff + UART_MAGIC_SIZE + UART_LENGTH_SIZE,
                                           iBuffLen - UART_CMD_SIZE - UART_SUBCMD_SIZE - UART_CHECKSUM_SIZE);

        OGM_PRINT_ORANGE(
            "FrameTotal:[%u] FrameNo:[%u] BuffLen:[%d] CRC:[0x%04X]\n",
            ucFrameTotal,
            g_ucFrameNo,
            iBuffLen,
            uiCRC);

        print_as_hexstring(1, (char *) pBuff, iBuffLen);
        write(g_nUsartfd, pBuff, iBuffLen);
        if (pBuff)
        {
            free(pBuff);
            pBuff = NULL;
        }

        switch (ReadDataFromUart())
        {
            case RECV_EOK:
                g_ucFrameNo++;
                g_uiTotalMapDataLeft -= iFrameDataBytesLen;
                g_uiMapDataSendOffset += iFrameDataBytesLen;
                break;
            case RECV_EGENERIC:
                pthread_exit((void *) 2);
            case RECV_ETIMEDOUT:
                OGM_PRINT_GREEN("Send again\n");
                break;
        }

    } while (g_uiTotalMapDataLeft > 0);

    OGM_PRINT_GREEN("Send Success\n");

    pthread_exit((void *) 0);
}

void *RecvThread(void *arg)
{
    fd_set rd;
    int    iReadLen = 0;
    int    state    = 0;
    int    recv_len = 0;
    int    len_temp = 0;

    printf("RecvThread...\n");

    while (1)
    {
        FD_ZERO(&rd);
        FD_SET(g_nUsartfd, &rd);
        if (FD_ISSET(g_nUsartfd, &rd))
        {
            if (select(g_nUsartfd + 1, &rd, NULL, NULL, NULL) < 0)
            {
                perror("select error\n");
            }
            else
            {
                if (state == 0)
                {
                    iReadLen = read(g_nUsartfd, g_ReadBuff, 1);
                    if (iReadLen == 1 && (*(g_ReadBuff) & 0xFF) == G_MAGIC_NUMBER)
                    {
                        state = 1;
                        recv_len += iReadLen;
                    }
                }
                else if (state == 1)
                {
                    iReadLen = read(g_nUsartfd, g_ReadBuff + recv_len, 2);
                    if (iReadLen == 1)
                    {
                        state    = 2;
                        len_temp = (*(g_ReadBuff + recv_len) << 8) * 0xFF00;
                        recv_len += iReadLen;
                    }
                    else if (iReadLen == 2)
                    {
                        state    = 3;
                        len_temp = ((*(g_ReadBuff + recv_len) << 8) & 0xFF00) + (*(g_ReadBuff + recv_len + 1) & 0xFF) + 1;
                        recv_len += iReadLen;
                    }
                }
                else if (state == 2)
                {
                    iReadLen = read(g_nUsartfd, g_ReadBuff + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state = 3;
                        len_temp += (*(g_ReadBuff + recv_len) & 0xFF) + 1;
                        recv_len += iReadLen;
                    }
                }
                else
                {
                    static int last = 0;
                    iReadLen = read(g_nUsartfd, g_ReadBuff + recv_len + last, len_temp - last);
                    if (iReadLen < (len_temp - last) && iReadLen > 0)
                    {
                        last += iReadLen;
                    }
                    else
                    {
                        recv_len += len_temp;
                        last = 0;

                        pthread_mutex_lock(&g_ReadMutex);
                        g_ReadBuffLen = recv_len;
                        print_as_hexstring(0, g_ReadBuff, g_ReadBuffLen);
                        pthread_cond_signal(&g_ReadCond);
                        pthread_mutex_unlock(&g_ReadMutex);

                        state    = 0;
                        recv_len = 0;
                    }
                }
            }
        }
    }
    pthread_exit((void *) 0);
}

void signal_handler(int signum)
{
    printf("Interrupt signal (%d) received.\n", signum);
    close(g_nUsartfd);

    OGM_PRINT_CYAN("Total recv count:[%llu]\nTotal error recv count:[%llu]\nTotal error recv per:[%.2f%%]\n",
                   g_ullTotalRecvCount,
                   g_ullTotalErrRecvCount,
                   g_ullTotalErrRecvCount * 1.0 / g_ullTotalRecvCount * 100);

    exit(signum);
}

void FillMapData()
{
    int i;
    for (i = 0; i < MAP_PIXELS * MAP_PIXELS / 2; i++)
    {
        g_Mapdata[i] = (unsigned char) (i);
    }
}

int main(int argc, char **argv)
{
//    int  nSpeed    = 115200;
//    int  nDatabits = 8;
//    int  nStopbits = 1;
//    char nParity   = 'n';
//    char *pDevice  = "/dev/ttyS1";
//
//    if (argc == 2)
//    {
//        pDevice = argv[1];
//    }
//    else if (argc == 3)
//    {
//        pDevice = argv[1];
//        nSpeed  = strtol(argv[2], NULL, 0);
//    }
//    else if (argc == 4)
//    {
//        pDevice = argv[1];
//        nSpeed  = strtol(argv[2], NULL, 0);
//        if (ReadMapDataFromFile(argv[3]))
//        {
//            exit(3);
//        }
//    }

    SerialOpts stSerialOpts = {
        .nSpeed    = 460800,
        .nDatabits = 8,
        .nStopbits = 1,
        .nParity   = 'n',
        .pDevice   = "/dev/ttyS1"
    };

    if ((g_nUsartfd = open_serial(&stSerialOpts)) < 0)
    {
        fprintf(stderr, "open serial failed\n");
        exit(1);
    }

    FillMapData();

    pthread_mutex_init(&g_ReadMutex, NULL);
    pthread_cond_init(&g_ReadCond, NULL);

    pthread_t TxID;
    pthread_t RxID;
    int       iRet;

    iRet = pthread_create(&RxID, NULL, RecvThread, NULL);
    if (iRet)
    {
        printf("pthread_create RecvThread failed\n");
        exit(-1);
    }

    iRet = pthread_create(&TxID, NULL, SendThread, NULL);
    if (iRet)
    {
        printf("pthread_create SendThread failed\n");
        exit(-1);
    }

    signal(SIGINT, signal_handler);

    pthread_join(TxID, NULL);
    pthread_join(RxID, NULL);

    return 0;
}
