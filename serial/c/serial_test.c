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

static const int PacketHeader  = 0xAA;
static const int MAX_LEN       = 1680;  //缓冲区最大长度
int              g_nUsartfd    = -1;

struct
{
    char buf[4096];
}                g_send_data[] = {
    // 向前
//    {"AA 03 02 21 00 23"},
//    {"AA 03 02 21 00 23"},
//    {"AA 03 02 21 00 23"},
//    {"AA 03 02 21 00 23"},
//    {"AA 03 02 21 00 23"},
//    {"AA 03 02 21 00 23"},

    // 自动清扫
//    {"AA 03 02 22 02 26"},
//    {"AA 03 02 22 02 26"},
//    {"AA 03 02 22 02 26"},
//    {"AA 03 02 22 02 26"},
//    {"AA 03 02 22 02 26"},
//    {"AA 03 02 22 02 26"},

//    {"AA 01 01 01"},
//    {"AA 03 02 10 00 12"},
//    {"AA 03 02 10 01 13"},
//    {"AA 03 02 21 00 23"},
//    {"AA 03 02 21 01 24"},
//    {"AA 03 02 21 02 25"},
//    {"AA 03 02 21 03 26"},
//    {"AA 03 02 22 00 24"},
//    {"AA 03 02 22 01 25"},
//    {"AA 03 02 22 02 26"},
//    {"AA 03 02 22 03 27"},
//    {"AA 03 02 32 01 35"},
//    {"AA 03 02 32 02 36"},

    // 海康板主动查询
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},
//    {"AA 00 01 06 06"},

//    {"AA 00 03 02 32 01 35"}, // 语音1
    {"AA 00 03 02 32 02 36"}, // 语音2

//    {"AA 00 01 83 83"},
//    {"AA 00 09 84 00 00 00 00 00 20 00 00 A4"},
//    {"AA 00 03 02 21 00 23"},
//    {"AA 00 11 03 01 01 40 00 00 01 00 01 00 01 00 01 00 20 00 00 69"},
//    {"74 68 69 73 20 61 20 68 65 6C 6C 6F 20 77 6F 72 6C 64 20 74 65 78 74"},
};

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

void *SendThread(void *arg)
{
    char *pSendData   = NULL;
    int  iSendDataLen = 0;
    int  i_g_send_data_num;
    int  i;

    printf("Start SendThread...\n");

    sleep(1);

    i_g_send_data_num = sizeof(g_send_data) / sizeof(g_send_data[0]);
//    printf("i_g_send_data_num:[%d]\n", i_g_send_data_num);
//    while (1)
//    {
        for (i = 0; i < i_g_send_data_num; i++)
        {
            if (hexstring_to_bytearray(g_send_data[i].buf, &pSendData, &iSendDataLen))
            {
                perror("HexStringToBytearray failed\n");
                pthread_exit((void *) 0);
            }

            OGM_PRINT_GREEN("Send Len:[%d]\n", iSendDataLen);
            OGM_PRINT_GREEN("Send: [");
            print_as_hexstring(pSendData, iSendDataLen);
//            printf("%s", g_send_data[i].buf);
//            printf("this a hello world text");
            OGM_PRINT_GREEN("]\n");

            write(g_nUsartfd, pSendData, iSendDataLen);

            if (pSendData)
            {
                free(pSendData);
                pSendData = NULL;
            }

            sleep(1);
//            usleep(1000);
        }
//    }
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

    printf("Start RecvThread...\n");

    while (1)
    {
        FD_ZERO(&rd);
        FD_SET(g_nUsartfd, &rd);
        memset(buf, 0, sizeof(buf));
        while (FD_ISSET(g_nUsartfd, &rd))
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
                    if (iReadLen == 1 && (*(buf) & 0xFF) == PacketHeader)
                    {
                        state = 1;
                        recv_len += iReadLen;
                    }
                    else
                    {
                        OGM_PRINT_RED("Recv wrong code:[0x%02X]\n", *buf);
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

                        state = 0;
                        OGM_PRINT_ORANGE("Rply:[");
                        print_as_hexstring((char *) buf, recv_len);
                        OGM_PRINT_ORANGE("]\n");
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
