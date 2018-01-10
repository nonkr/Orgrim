/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/4 16:35
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include "../../color.h"

void SignalHandler(int signum)
{
    OGM_PRINT_CYAN("Interrupt signal (%d) received.\n", signum);
    exit(signum);
}

int main()
{
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGKILL, SignalHandler);

    //创建套接字
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family      = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port        = htons(4433);  //端口
    connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    while (1)
    {
        char   *line = NULL;
        size_t size;
        if (getline(&line, &size, stdin) == -1)
        {
            OGM_PRINT_ORANGE("no line\n");
        }
        else
        {
            line[strlen(line) - 1] = '\0';
            if (!strcmp(line, "quit"))
            {
                break;
            }
            else
            {
                if (write(sock_fd, line, strlen(line)) < 0)
                {
                    perror("write");
                    return -1;
                }
            }
        }
    }

    //关闭套接字
    close(sock_fd);

    return 0;
}
