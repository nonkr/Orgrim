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
#include <arpa/inet.h>

int main()
{
    int server_fd;
    int client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // 创建套接字
    if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("create socket failed\n");
        exit(1);
    }

    // 将套接字和IP、端口绑定
    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family = AF_INET;  //使用IPv4地址
    server_addr.sin_addr.s_addr = INADDR_ANY;  //具体的IP地址
    server_addr.sin_port = htons(1234);  //端口

    // 设置套接字选项避免地址使用错误
    int on                            = 1;
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

    // 接收客户端请求
    if ((client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size)) == -1)
    {
        perror("accept socket failed\n");
        exit(4);
    }

    // 向客户端发送数据
    char str[] = "Hello World!";
    write(client_fd, str, sizeof(str));

    // 关闭套接字
    close(client_fd);
    close(server_fd);

    return 0;
}
