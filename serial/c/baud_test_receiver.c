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
#include <signal.h>
#include <inttypes.h>
#include "../../color.h"
#include "serial.h"

#define UART_BUFF_MEX_LEN 1680
static const int G_MAGIC_NUMBER = 0xAA;
static const int MAX_LEN        = UART_BUFF_MEX_LEN;  //缓冲区最大长度

int g_nUsartfd;

static unsigned long long g_ullTotalRecvCount    = 0ULL;
static unsigned long long g_ullTotalErrRecvCount = 0ULL;

static int g_iSenderBuffLen;

struct Mcu2HostFastReg
{
    //里程计积分量
    int32_t  opticalFlowX;//光流x
    int32_t  opticalFlowY;//光流y
    int32_t  leftPos;     //左轮
    int32_t  rightPos;    //右轮
    int32_t  yawPos;      //水平面角度积分
    //单点tof，psd
    uint16_t psdLeft;
    uint16_t psdRight;
    uint16_t psdFront;
    //底层解算的位姿
    int32_t  odoX;
    int32_t  odoY;
    int32_t  odoPhi;
    //倾斜角
    int32_t  pitch;
    int32_t  roll;
    //实测速度
    int32_t  v;
    int32_t  w;
    //lidar tof
    uint16_t lidarLeft;
    uint16_t lidarRight;
};

void print_as_hexstring(const int iIsSend, const unsigned char *pData, int iDataLen)
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

void DecodeBigBuffer(unsigned char *pBuff, const int iBuffLen)
{
    unsigned short usUartLen = g_iSenderBuffLen - 4;
    int            i;

//    print_as_hexstring(0, pBuff, iBuffLen);

    g_ullTotalRecvCount++;

    if (iBuffLen < 6 ||
        pBuff[0] != 0xAA ||
        pBuff[1] != (usUartLen >> 8) ||
        pBuff[2] != (usUartLen & 0xFF) ||
        pBuff[3] != 0xBB)
    {
        g_ullTotalErrRecvCount++;
        goto end;
    }

    for (i = 4; i < usUartLen; i++)
    {
        if (pBuff[i] != ((i - 4) & 0xFF))
        {
            g_ullTotalErrRecvCount++;
            goto end;
        }
    }

    if (pBuff[iBuffLen - 1] != CalcChecksum(pBuff + 3, usUartLen))
    {
        g_ullTotalErrRecvCount++;
        goto end;
    }

end:
    OGM_PRINT_CYAN("TotalRecv:[%llu] TotalErrorRecv:[%llu/%.2f%%]\n",
                   g_ullTotalRecvCount,
                   g_ullTotalErrRecvCount,
                   g_ullTotalErrRecvCount * 1.0 / g_ullTotalRecvCount * 100);
}

void *RecvThread(void *arg)
{
    fd_set        rd;
    int           iReadLen = 0;
    unsigned char buf[MAX_LEN];
    int           i;

    printf("RecvThread...\n");

    while (1)
    {
        FD_ZERO(&rd);
        FD_SET(g_nUsartfd, &rd);
        memset(buf, 0x00, sizeof(buf));
        if (FD_ISSET(g_nUsartfd, &rd))
        {
            if (select(g_nUsartfd + 1, &rd, NULL, NULL, NULL) < 0)
            {
                perror("select error\n");
            }
            else
            {
                iReadLen = read(g_nUsartfd, buf, 60);
//                printf("iReadLen:%d\n", iReadLen);
                for (i   = 0; i < iReadLen; i++)
                {
                    if (buf[i] != 0x55)
                    {
                        printf("0x%02X ", buf[i]);
                    }
                }
//                printf("\n");
                fflush(stdout);
            }
        }
    }
    pthread_exit((void *) 0);
}

void signal_handler(int signum)
{
    printf("Interrupt signal (%d) received.\n", signum);
    close(g_nUsartfd);

    OGM_PRINT_CYAN("TotalRecv:[%llu]\nTotalErrorRecv:[%llu/%.2f%%]\n",
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

    if (argc < 2)
    {
        printf("Usage: %s baud\n", argv[0]);
        exit(2);
    }

    nSpeed = strtol(argv[1], NULL, 0);

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

    pthread_t RxID;
    int       iRet;

    printf("%zu\n", sizeof(struct Mcu2HostFastReg));

    iRet = pthread_create(&RxID, NULL, RecvThread, NULL);
    if (iRet)
    {
        printf("pthread_create RecvThread failed\n");
        exit(-1);
    }

    signal(SIGINT, signal_handler);

    pthread_join(RxID, NULL);

    return 0;
}
