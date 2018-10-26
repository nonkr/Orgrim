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
#include "../../color.h"
#include "serial.h"

#define UART_BUFF_MEX_LEN 1680
static const int G_MAGIC_NUMBER = 0xAA;
static const int MAX_LEN      = UART_BUFF_MEX_LEN;  //缓冲区最大长度

int g_nUsartfd;

pthread_mutex_t g_ReadMutex;
pthread_cond_t  g_ReadCond;
static char     g_ReadBuff[UART_BUFF_MEX_LEN];
static int      g_ReadBuffLen = 0;

static unsigned long long g_ullTotalRecvCount    = 0ULL;
static unsigned long long g_ullTotalErrRecvCount = 0ULL;

//#define FIRMWARE_FRAME_DATA "AA 01 CD 85 0E BF 00 19 E9 FC 01 CA FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF C0 0D 26"
//#define FIRMWARE_FRAME_DATA "AA 00 6F 85 00 01 00 00 00 00 00 6C 51 E9 01 00 F9 01 00 F9 01 00 92 13 00 92 13 00 97 57 00 97 57 00 16 6B 00 16 6B 00 83 7C 00 83 7C 00 D1 10 01 D1 10 01 69 1D 01 69 1D 01 AD 8A 01 AD 8A 01 06 FA 01 06 FA 01 2D B0 02 2D B0 02 B3 2A 03 B3 2A 03 20 75 03 20 75 03 11 84 03 11 84 03 EE 94 03 EE 94 03 76 AA 03 76 AA 03 74 CE 03 74 CE 03 9D 6C 4C"
#define FIRMWARE_FRAME_DATA "AA 01 9B 85 08 7A 00 0D 3D 10 01 98 19 BE A8 81 53 28 AB CB 91 34 42 19 DB BA 04 52 10 9B CB 90 36 31 88 CC B8 12 43 30 BC CA 90 25 42 89 CB A9 03 53 10 9C DA 91 14 42 0A AC BA 04 34 21 AC BC 90 24 42 09 BD AA 12 43 30 AC CB A1 33 63 19 DB AA 83 53 30 9D BB A8 35 43 08 BD BA 02 43 40 9B CB A0 35 32 19 CC AA 02 44 21 9B EA 90 14 33 0A BC CB 83 53 31 8D BB A8 25 42 18 CB BA 92 54 30 9A CC 98 23 43 1A BB EA 83 43 31 9C DA A0 14 33 28 CD B9 82 35 21 8A DB B0 24 33 39 DB CA 81 53 22 8B DB B8 24 43 18 BD BA 92 44 32 8C BC B0 24 33 28 CC CA 81 43 32 8B FA A8 13 53 08 AC BB 82 44 32 8B DC A8 04 33 28 AE AA 91 35 31 89 DB A9 23 44 28 BC BB 92 44 32 0C CA B9 14 52 28 AD BA 81 35 31 0A DB B8 14 42 28 BC BB 92 53 41 0B DB 99 23 62 08 AB D9 81 34 32 8C CB A8 14 43 18 BD BA 92 44 32 8B DC 98 13 43 18 BC CA 92 35 31 8B CC A8 14 34 18 AC C9 81 34 31 8B DC A8 23 43 18 BE AA 81 44 21 8B CC 98 14 33 19 BD BB 82 45 21 8A CB A9 24 52 18 BC BA 81 45 21 8B CB A9 24 52 18 BC BA 82 44 21 8B CC A8 24 42 19 BC BB 02 54 21 9A DA A8 23 53 19 BD BA 82 44 31 8C BC A8 14 42 29 AC CA 81 43 32 9B DC A8 23 52 18 BC BB 01 45 21 8A DA A8 23 62 19 AD AA 02 34 31 9B EB 98 15 32 19 BD BA 02 44 30 8C BC A0 24 42 19 BD BA 02 44 21 9B CB A8 25 42 19 BD B9 02 25 58 B8"

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
            switch (pData[2] & 0xFF)
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

void ReadDataFromUart()
{
    pthread_mutex_lock(&g_ReadMutex);
    while (g_ReadBuffLen <= 0)
    {
        pthread_cond_wait(&g_ReadCond, &g_ReadMutex);
    }

    if (g_ReadBuffLen >= 6)
    {
        if (g_ReadBuff[3] == 0x85)
        {
            g_ullTotalRecvCount++;
            if (isChecksumError(g_ReadBuff, g_ReadBuffLen) ||
                g_ReadBuff[4] != 0x00)
            {
                g_ullTotalErrRecvCount++;
            }
        }
    }

    g_ReadBuffLen = 0;
    pthread_mutex_unlock(&g_ReadMutex);
}

void *SendThread(void *arg)
{
    printf("SendThread...\n");

    sleep(1);

    char *pSendBuff = NULL;
    int  iBuffLen   = 0;
//    if (hexstring_to_bytearray("AA 00 01 83 83", &pSendBuff, &iBuffLen))
//    {
//        perror("HexStringToBytearray failed\n");
//        pthread_exit((void *) 0);
//    }
//    print_as_hexstring(1, pSendBuff, iBuffLen);
//    write(g_nUsartfd, pSendBuff, iBuffLen);
//    free(pSendBuff);
//
//    ReadDataFromUart();
//
//    if (hexstring_to_bytearray("AA 00 09 84 00 00 00 00 00 20 00 00 A4", &pSendBuff, &iBuffLen))
//    {
//        perror("HexStringToBytearray failed\n");
//        pthread_exit((void *) 0);
//    }
//    print_as_hexstring(1, pSendBuff, iBuffLen);
//    write(g_nUsartfd, pSendBuff, iBuffLen);
//    free(pSendBuff);
//
//    ReadDataFromUart();


    while (1)
    {
        if (hexstring_to_bytearray(FIRMWARE_FRAME_DATA, &pSendBuff, &iBuffLen))
        {
            perror("HexStringToBytearray failed\n");
            pthread_exit((void *) 0);
        }
        OGM_PRINT_GREEN("Send:[%d]\n", iBuffLen);
        write(g_nUsartfd, pSendBuff, iBuffLen);
        free(pSendBuff);

        ReadDataFromUart();
    }

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
        memset(g_ReadBuff, 0, sizeof(g_ReadBuff));
        while (FD_ISSET(g_nUsartfd, &rd))
        {
            if (select(g_nUsartfd + 1, &rd, NULL, NULL, NULL) < 0)
            {
                perror("select error\n");
            }
            else
            {
//                iReadLen = read(g_nUsartfd, g_ReadBuff, MAX_LEN);
//                OGM_PRINT_ORANGE("read_len:[%d]\n", iReadLen);
//                OGM_PRINT_ORANGE("Reply:[");
//                print_as_hexstring("Recv", g_ReadBuff, iReadLen);
//                OGM_PRINT_ORANGE("]\n\n");

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
                        len_temp = (*(g_ReadBuff + recv_len) << 8);
                        recv_len += iReadLen;
                    }
                    else if (iReadLen == 2)
                    {
                        state    = 3;
                        len_temp = (*(g_ReadBuff + recv_len) << 8) + *(g_ReadBuff + recv_len + 1) + 1;
                        recv_len += iReadLen;
                    }
                }
                else if (state == 2)
                {
                    iReadLen = read(g_nUsartfd, g_ReadBuff + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state = 3;
                        len_temp += *(g_ReadBuff + recv_len) + 1;
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

int main(int argc, char **argv)
{
    int  nSpeed    = 115200;
    int  nDatabits = 8;
    int  nStopbits = 1;
    char nParity   = 'n';
    char *pDevice  = "/dev/ttyS1";

    if (argc == 2)
    {
        pDevice = argv[1];
    }
    else if (argc == 3)
    {
        pDevice = argv[1];
        nSpeed  = strtol(argv[2], NULL, 0);
    }

    printf("use tty:%s baud:%d\n", pDevice, nSpeed);

    g_nUsartfd = open(pDevice, O_RDWR | O_NOCTTY | O_NDELAY);
    if (g_nUsartfd == -1)
    {
        perror("open serial failed");
        return -1;
    }
    if (set_speed(g_nUsartfd, nSpeed) < 0)
    {
        printf("set_speed failed\n");
        return -1;
    }
    if (set_Parity(g_nUsartfd, nDatabits, nStopbits, nParity) < 0)
    {
        printf("set_Parity failed\n");
        return -1;
    }

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
