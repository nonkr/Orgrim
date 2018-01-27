/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
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
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include "../../color.h"

#define UART_BUFF_MEX_LEN 1680
static const int PacketHeader = 0xAA;
static const int MAX_LEN      = UART_BUFF_MEX_LEN;  //缓冲区最大长度

int speed_arr[] = {B1500000, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400,
                   B19200,
                   B9600, B4800,
                   B2400, B1200, B300,};
int name_arr[]  = {1500000, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600,
                   4800,
                   2400, 1200,
                   300,};

typedef unsigned char UINT8;

int  m_Usartfd;
int  m_nSpeed;
int  m_nDatabits;
int  m_nStopbits;
char m_nParity;

static unsigned long long g_ullTotalRecvCount    = 0ULL;
static unsigned long long g_ullTotalErrRecvCount = 0ULL;

static int g_iSenderBuffLen;

static int set_speed()
{
    UINT8          i;
    int            status;
    struct termios Opt;
    tcgetattr(m_Usartfd, &Opt);
    for (i = 0; i < (sizeof(speed_arr) / sizeof(speed_arr[0])); i++)
    {
        if (m_nSpeed == name_arr[i])
        {
            tcflush(m_Usartfd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(m_Usartfd, TCSANOW, &Opt);
            if (status != 0)
            {
                perror("tcsetattr fd1");
                return -1;
            }
            tcflush(m_Usartfd, TCIOFLUSH);
        }
    }
    return 0;
}

static int set_Parity()
{
    struct termios options;
    if (tcgetattr(m_Usartfd, &options) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (m_nDatabits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return -1;
    }
    switch (m_nParity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;    /* Clear parity enable */
            options.c_iflag &= ~INPCK;    /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;    /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;    /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;    /* Disnable parity checking */
            break;
        case 'S':
        case 's':            /*as no parity */
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return -1;
    }

    switch (m_nStopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return -1;
    }

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    options.c_oflag &= ~OPOST;                          /*Output*/

    // reference: https://linux.die.net/man/3/cfmakeraw
    cfmakeraw(&options);

    /* Set input parity option */
    if (m_nParity != 'n')
        options.c_iflag |= INPCK;
    tcflush(m_Usartfd, TCIFLUSH);
    options.c_cc[VTIME] = 150;
    options.c_cc[VMIN]  = 0;    /* Update the options and do it NOW */
    if (tcsetattr(m_Usartfd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}


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
    OGM_PRINT_CYAN("Total recv count:[%llu] Total error recv count:[%llu] Total error recv per:[%.2f%%]\n",
                   g_ullTotalRecvCount,
                   g_ullTotalErrRecvCount,
                   g_ullTotalErrRecvCount * 1.0 / g_ullTotalRecvCount * 100);
}

void *RecvThread(void *arg)
{
    fd_set        rd;
    int           iReadLen = 0;
    unsigned char buf[MAX_LEN];
    int           state    = 0;
    int           recv_len = 0;
    int           len_temp = 0;

    printf("RecvThread...\n");

    while (1)
    {
        FD_ZERO(&rd);
        FD_SET(m_Usartfd, &rd);
        memset(buf, 0x00, sizeof(buf));
        while (FD_ISSET(m_Usartfd, &rd))
        {
            if (select(m_Usartfd + 1, &rd, NULL, NULL, NULL) < 0)
            {
                perror("select error\n");
            }
            else
            {
//                iReadLen = read(m_Usartfd, buf, MAX_LEN);
//                OGM_PRINT_ORANGE("read_len:[%d]\n", iReadLen);
//                print_as_hexstring(0, (char *) buf, iReadLen);

                if (state == 0)
                {
                    iReadLen = read(m_Usartfd, buf, 1);
                    if (iReadLen == 1 && (*(buf) & 0xFF) == PacketHeader)
                    {
                        state = 1;
                        recv_len += iReadLen;
                    }
                }
                else if (state == 1)
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 2);
                    if (iReadLen == 1)
                    {
                        state    = 2;
                        len_temp = (*(buf + recv_len) << 8) & 0xFF00;
                        recv_len += iReadLen;
                        OGM_PRINT_BLUE("LEN1:0x%02X\n", buf[1]);
                    }
                    else if (iReadLen == 2)
                    {
                        state    = 3;
                        len_temp = ((*(buf + recv_len) << 8) & 0xFF00) + (*(buf + recv_len + 1) & 0xFF) + 1;
                        recv_len += iReadLen;
                        OGM_PRINT_BLUE("LEN1:0x%02X\n", buf[1]);
                        OGM_PRINT_BLUE("LEN2:0x%02X\n", buf[2]);
                        // TODO: 对读取的长度进行判断，不能超过预设的buffer长度
                    }
                }
                else if (state == 2)
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state = 3;
                        len_temp += (*(buf + recv_len) & 0xFF) + 1;
                        recv_len += iReadLen;
                        OGM_PRINT_BLUE("LEN2:0x%02X\n", buf[2]);
                    }
                }
                else
                {
                    static int last = 0;
//                    printf("len_temp:[%d] last:[%d]\n", len_temp, last);
                    iReadLen = read(m_Usartfd, buf + recv_len + last, len_temp - last);
                    if (iReadLen < (len_temp - last) && iReadLen > 0)
                    {
                        last += iReadLen;
                    }
                    else
                    {
                        recv_len += len_temp;
                        last = 0;

                        state = 0;
//                        print_as_hexstring(0, (char *) buf, recv_len);
                        DecodeBigBuffer(buf, recv_len);
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
    close(m_Usartfd);

    OGM_PRINT_CYAN("Total recv count:[%llu]\nTotal error recv count:[%llu]\nTotal error recv per:[%.2f%%]\n",
                   g_ullTotalRecvCount,
                   g_ullTotalErrRecvCount,
                   g_ullTotalErrRecvCount * 1.0 / g_ullTotalRecvCount * 100);

    exit(signum);
}

int main(int argc, char **argv)
{
    m_Usartfd   = -1;
//    m_nSpeed    = 115200;
//    m_nSpeed    = 1500000;
    m_nDatabits = 8;
    m_nStopbits = 1;
    m_nParity   = 'n';

    if (argc < 3)
    {
        printf("Usage: %s /dev/ttyS1 baud bufflen\n", argv[0]);
        exit(2);
    }

    m_nSpeed = atol(argv[2]);
    g_iSenderBuffLen = atoi(argv[3]);

    m_Usartfd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_Usartfd == -1)
    {
        perror("can't open serial port,UsartInit failed!");
        return -1;
    }
    if (set_speed() < 0)
    {
        printf("set_speed failed!\n");
        return -1;
    }
    if (set_Parity() < 0)
    {
        printf("set_Parity failed!\n");
        return -1;
    }

    pthread_t RxID;
    int       iRet;

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