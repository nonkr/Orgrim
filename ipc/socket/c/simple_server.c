/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/28 23:31
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#define LISTEN_PORT (1234)

void handleClient(int fd)
{
    fd_set read_sd;
    FD_ZERO(&read_sd);
    FD_SET(fd, &read_sd);

    while (1)
    {
        fd_set rsd = read_sd;

        int sel = select(fd + 1, &rsd, 0, 0, 0);
        if (sel > 0)
        {
            // client has performed some activity (sent data or disconnected?)
            char buf[1024] = {0};

            int bytes = read(fd, buf, sizeof(buf));
            if (bytes > 0)
            {
                // got data from the client.
                printf("Message form server: %s\n", buf);
            }
            else if (bytes == 0)
            {
                // client disconnected.
                break;
            }
            else
            {
                // error receiving data from client. You may want to break from
                // while-loop here as well.
            }
        }
        else if (sel < 0)
        {
            // grave error occurred.
            break;
        }
    }

    close(fd);
}

void signalHandler(int signum)
{
    // TODO: to close socket softly
    exit(signum);
}

int main()
{
    int                server_fd;
    int                client_fd;
    struct sockaddr_in server_addr;

    signal(SIGINT, signalHandler);

    // 创建套接字
    if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("create socket failed\n");
        exit(1);
    }

    // 将套接字和IP、端口绑定
    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family      = AF_INET;  //使用IPv4地址
    server_addr.sin_addr.s_addr = INADDR_ANY;  //具体的IP地址
    server_addr.sin_port        = htons(LISTEN_PORT);  //端口

    // 设置套接字选项避免地址使用错误
    int on = 1;
    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if ((bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1)
    {
        perror("bind socket failed\n");
        exit(2);
    }

    // 进入监听状态，等待用户发起请求
    if ((listen(server_fd, 20)) == -1)
    {
        perror("listen socket failed\n");
        exit(3);
    }
    printf("server listen on port:%u\n", LISTEN_PORT);

    while (1)
    {
        // 接收客户端请求
        if ((client_fd = accept(server_fd, 0, 0)) > 0)
        {
            printf("accpet client\n");
            handleClient(client_fd);
        }
    }

    close(server_fd);

    return 0;
}
