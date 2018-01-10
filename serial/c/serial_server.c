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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../color.h"

#define MAX_SENDBUFF_LEN (1025 * 16)
#define MAX_SOCKET_READ_LEN (MAX_SENDBUFF_LEN * 3)
static const int PacketHeader = 0xAA;
static const int MAX_LEN      = MAX_SENDBUFF_LEN;  //缓冲区最大长度

int speed_arr[]                = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200,
                                  B9600, B4800,
                                  B2400, B1200, B300,};
int name_arr[]                 = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800,
                                  2400, 1200,
                                  300,};

typedef unsigned char UINT8;

int  m_Usartfd;
int  m_nSpeed;
int  m_nDatabits;
int  m_nStopbits;
char m_nParity;

pthread_mutex_t g_mutex_sendbuff;
pthread_cond_t  g_cond_sendbuff;
static int      g_sendbuff_len = 0;
static char     g_sendbuff[MAX_SENDBUFF_LEN];

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
    options.c_iflag &= ~IXON; // Enable XON/XOFF flow control on output.

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
        if (i >= 2 && i < iDataLen - 1)
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
        else if (i == 1)
        {
            OGM_PRINT_ORANGE("%02X ", pData[i] & 0xFF);
        }
        else if (i == 2)
        {
            OGM_PRINT_BLUE("%02X ", pData[i] & 0xFF);
        }
        else if (i == 3)
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
    int         writeLen;
    static char tmpbuff[MAX_SOCKET_READ_LEN];

    while (1)
    {
        pthread_mutex_lock(&g_mutex_sendbuff);

        while (g_sendbuff_len <= 0)
        {
            printf("before pthread_cond_wait\n");
            pthread_cond_wait(&g_cond_sendbuff, &g_mutex_sendbuff);
            printf("after pthread_cond_wait\n");
        }

        if (g_sendbuff_len >= MAX_LEN)
        {
            g_sendbuff_len = 0;
            OGM_PRINT_RED("Send buffer is too long\n");
        }
        else
        {
            printf("before write\n");
            writeLen = write(m_Usartfd, g_sendbuff, g_sendbuff_len);
            printf("after write len:[%d]\n", writeLen);

            if (writeLen < 0)
            {
                OGM_PRINT_RED("Can not write buffer to serial port\n");
                pthread_exit((void *) 0);
            }
            else
            {
                OGM_PRINT_GREEN("Send: [");
                print_as_hexstring(g_sendbuff, writeLen);
                OGM_PRINT_GREEN("]\n");

                if (writeLen == g_sendbuff_len)
                {
                    g_sendbuff_len = 0;
                    memset(g_sendbuff, 0x00, MAX_SENDBUFF_LEN);
                }
                else if (writeLen < g_sendbuff_len)
                {
                    g_sendbuff_len -= writeLen;
                    memset(tmpbuff, 0x00, MAX_SENDBUFF_LEN);
                    memcpy(tmpbuff, g_sendbuff + writeLen, g_sendbuff_len);
                    memcpy(g_sendbuff, tmpbuff, g_sendbuff_len);
                }
            }
        }

        pthread_mutex_unlock(&g_mutex_sendbuff);
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
        FD_SET(m_Usartfd, &rd);
        memset(buf, 0, sizeof(buf));
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
//                OGM_PRINT_ORANGE("Reply:[");
//                print_as_hexstring((char *) buf, iReadLen);
//                OGM_PRINT_ORANGE("]\n\n");

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
                    iReadLen = read(m_Usartfd, buf + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state    = 2;
                        len_temp = *(buf + recv_len) + 1;
                        recv_len += iReadLen;
                    }
                }
                else
                {
                    static int last = 0;
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

void *ServerSocketThread(void *arg)
{
    //创建套接字
    int server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将套接字和IP、端口绑定
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family      = AF_INET;  //使用IPv4地址
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    server_addr.sin_port        = htons(4433);  //端口
    bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    //进入监听状态，等待用户发起请求
    listen(server_fd, 20);

    //接收客户端请求
    struct sockaddr_in client_addr;

    socklen_t client_addr_size = sizeof(client_addr);
    int       client_fd        = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

    int         i_readlen;
    static char readbuff[MAX_SENDBUFF_LEN];

    while (1)
    {
        OGM_PRINT_GREEN("wait...\n");
        if ((i_readlen = read(client_fd, readbuff, sizeof(readbuff) - 1)) < 0)
        {
            perror("read");
            pthread_exit((void *) 1);
        }
        if (i_readlen <= 0)
        {
            break;
        }
        readbuff[i_readlen] = '\0';
        printf("read:[%s]\n", readbuff);

        char *p_buffer    = NULL;
        int  i_buffer_len = 0;

        if (hexstring_to_bytearray(readbuff, &p_buffer, &i_buffer_len))
        {
            OGM_PRINT_RED("convert failed:[%s]\n", readbuff);
            continue;
        }

        pthread_mutex_lock(&g_mutex_sendbuff);

        memcpy(g_sendbuff + g_sendbuff_len, p_buffer, i_buffer_len);
        g_sendbuff_len += i_buffer_len;

        pthread_cond_broadcast(&g_cond_sendbuff);
        pthread_mutex_unlock(&g_mutex_sendbuff);

        if (p_buffer)
        {
            free(p_buffer);
            p_buffer = NULL;
        }
    }

    //关闭套接字
    close(client_fd);
    close(server_fd);

    pthread_exit((void *) 0);
}

int main(int argc, char **argv)
{
    m_Usartfd   = -1;
    m_nSpeed    = 115200;
    m_nDatabits = 8;
    m_nStopbits = 1;
    m_nParity   = 'n';

    if (argc < 2)
    {
        printf("Usage: %s /dev/ttyS1\n", argv[0]);
        exit(2);
    }

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

    pthread_t TxID;
    pthread_t RxID;
    pthread_t ServerSocketID;
    int       iRet;

    pthread_mutex_init(&g_mutex_sendbuff, NULL);
    pthread_cond_init(&g_cond_sendbuff, NULL);

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

    iRet = pthread_create(&ServerSocketID, NULL, ServerSocketThread, NULL);
    if (iRet)
    {
        printf("pthread_create ServerSocketThread failed\n");
        exit(-1);
    }

    pthread_join(RxID, NULL);
    pthread_join(TxID, NULL);
    pthread_join(ServerSocketID, NULL);

    return 0;
}