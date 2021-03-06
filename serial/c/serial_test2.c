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

static const int G_MAGIC_NUMBER     = 0xAA;
static const int MAX_LEN          = 1680;  //缓冲区最大长度

#define CRC_TABLE_SIZE 256
#define CRC_GEN_POLY   0x8005
static unsigned int m_CRCTable[CRC_TABLE_SIZE];


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

unsigned int BlockCRC16TableFor8813(const unsigned char *Block, unsigned int Len,
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

unsigned int GenCRC(const unsigned char *Block, unsigned int Len)
{
    return BlockCRC16TableFor8813(Block, Len, 0, 0);
}

int  g_nUsartfd;

static char     g_SendBuffer[1024];
static int      g_SendBufferCount = 0;
pthread_mutex_t g_SendBuffer_Mutex;
pthread_cond_t  g_SendBuffer_Cond;

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

void print_as_hexstring(const char *pData, int iDataLen)
{
    unsigned char check_sum = 0;
    int           i;

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
}

void ProcessRecvBuffer(const char *pBuff, const unsigned int uiBuffLen)
{
    OGM_PRINT_ORANGE("Recv:[");
    print_as_hexstring(pBuff, uiBuffLen);
    OGM_PRINT_ORANGE("]\n");

    int iCRCRecv = 0;
    int iCRCCalc = 0;

    if (uiBuffLen < 5)
    {
        OGM_PRINT_RED("Malformed received data\n");
        return;
    }

    int cCmd = pBuff[3] & 0xFF;
    int iSendBuffLen;
    g_SendBuffer[0] = (char) 0xAA;
    g_SendBuffer[1] = (char) 0x00;

//    OGM_PRINT_CYAN("cCmd:[0x%02X]\n", cCmd);
    switch (cCmd)
    {
        case 0x83:
            g_SendBuffer[2] = (char) 0x02;
            g_SendBuffer[3] = (char) 0x83;
            g_SendBuffer[4] = (char) 0x00;
            g_SendBuffer[5] = (char) 0x83;
            iSendBuffLen = 6;
            break;
        case 0x84:
            g_SendBuffer[2] = (char) 0x01;
            g_SendBuffer[3] = (char) 0x84;
            g_SendBuffer[4] = (char) 0x84;
            iSendBuffLen = 5;
            break;
        case 0x85:
            g_SendBuffer[2] = (char) 0x02;
            g_SendBuffer[3] = (char) 0x85;
            g_SendBuffer[5] = (char) 0x85;
            iSendBuffLen = 6;

            iCRCRecv = ((pBuff[uiBuffLen - 3] << 8) & 0xFF00) + (pBuff[uiBuffLen - 2] & 0xFF);
            iCRCCalc = GenCRC((unsigned char *) pBuff + 6, uiBuffLen - 9);

            if (iCRCCalc != iCRCRecv)
            {
                g_SendBuffer[4] = (char) 0x01;
                OGM_PRINT_ORANGE("iCRCRecv:[0x%04X] iCRCCalc:[0x%04X]\n", iCRCRecv, iCRCCalc);
            }
            else
            {
                g_SendBuffer[4] = (char) 0x00;
            }

            break;
        case 0x86:
            g_SendBuffer[2] = (char) 0x02;
            g_SendBuffer[3] = (char) 0x86;
            g_SendBuffer[4] = (char) 0x00;
            g_SendBuffer[5] = (char) 0x86;
            iSendBuffLen = 6;
            break;
        default:
            iSendBuffLen = 0;
    }

    if (iSendBuffLen > 0)
    {
        pthread_mutex_lock(&g_SendBuffer_Mutex);
        g_SendBufferCount = iSendBuffLen;
        pthread_cond_signal(&g_SendBuffer_Cond);
        pthread_mutex_unlock(&g_SendBuffer_Mutex);
    }
}

void *SendThread(void *arg)
{
    printf("SendThread...\n");

    while (1)
    {
        pthread_mutex_lock(&g_SendBuffer_Mutex);

        if (g_SendBufferCount <= 0)
        {
            pthread_cond_wait(&g_SendBuffer_Cond, &g_SendBuffer_Mutex);
        }

//        OGM_PRINT_GREEN("g_SendBuffer:[%d]\n", g_SendBufferCount);
        OGM_PRINT_GREEN("Send: [");
        print_as_hexstring(g_SendBuffer, g_SendBufferCount);
        OGM_PRINT_GREEN("]\n");

        write(g_nUsartfd, g_SendBuffer, g_SendBufferCount);

        g_SendBufferCount = 0;

        pthread_mutex_unlock(&g_SendBuffer_Mutex);
    }

    pthread_exit((void *) 0);
}

void *RecvThread(void *arg)
{
    int    iReadLen;
    fd_set rd;
    char   buf[MAX_LEN];
    int    state    = 0;
    int    recv_len = 0;
    int    len_temp = 0;

    printf("RecvThread...\n");

    while (1)
    {
        FD_ZERO(&rd);
        FD_SET(g_nUsartfd, &rd);
        memset(buf, 0, sizeof(buf));
        if (FD_ISSET(g_nUsartfd, &rd))
        {
            if (select(g_nUsartfd + 1, &rd, NULL, NULL, NULL) < 0)
            {
                perror("select error\n");
            }
            else
            {
//                iReadLen = read(g_nUsartfd, buf, MAX_LEN);
//                OGM_PRINT_ORANGE("read_len:[%d]\n", iReadLen);
//                OGM_PRINT_ORANGE("Reply:[");
//                print_as_hexstring((char *) buf, iReadLen);
//                OGM_PRINT_ORANGE("]\n\n");

                if (state == 0)
                {
                    iReadLen = read(g_nUsartfd, buf, 1);
                    if (iReadLen == 1 && (*(buf) & 0xFF) == G_MAGIC_NUMBER)
                    {
                        state = 1;
                        recv_len += iReadLen;
                    }
                }
                else if (state == 1)
                {
                    iReadLen = read(g_nUsartfd, buf + recv_len, 2);
                    if (iReadLen == 1)
                    {
                        state    = 2;
                        len_temp = (*(buf + recv_len) << 8) & 0xFF00;
                        recv_len += iReadLen;
                    }
                    else if (iReadLen == 2)
                    {
                        state    = 3;
                        len_temp = ((*(buf + recv_len) << 8) & 0xFF00) + (*(buf + recv_len + 1) & 0xFF) + 1;
                        recv_len += iReadLen;
                    }
                }
                else if (state == 2)
                {
                    iReadLen = read(g_nUsartfd, buf + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state = 3;
                        len_temp += (*(buf + recv_len) & 0xFF) + 1;
                        recv_len += iReadLen;
                    }
                }
                else
                {
//                    OGM_PRINT_LIMEGREEN("len_temp:[0x%04X]\n", len_temp - 1);
                    static int last = 0;
                    iReadLen = read(g_nUsartfd, buf + recv_len + last, len_temp - last);
                    if (iReadLen < (len_temp - last) && iReadLen > 0)
                    {
                        last += iReadLen;
                    }
                    else
                    {
                        recv_len += len_temp;
                        last = 0;

                        ProcessRecvBuffer(buf, recv_len);

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
    pthread_mutex_destroy(&g_SendBuffer_Mutex);
    pthread_cond_destroy(&g_SendBuffer_Cond);

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

    BuildCRC16TableFor8813(CRC_GEN_POLY);

    pthread_t TxID;
    pthread_t RxID;
    int       iRet;

    pthread_mutex_init(&g_SendBuffer_Mutex, NULL);
    pthread_cond_init(&g_SendBuffer_Cond, NULL);

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

    pthread_mutex_destroy(&g_SendBuffer_Mutex);
    pthread_cond_destroy(&g_SendBuffer_Cond);

    return 0;
}
