/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/28 23:32
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int                server_fd;
    struct sockaddr_in server_addr;

    // 创建套接字
    if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("create socket failed\n");
        exit(1);
    }

    // 向服务器（特定的IP和端口）发起请求
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(1234);
    if ((connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1)
    {
        perror("connect failed\n");
        exit(2);
    }

    char str[] = "Hello World!";
    write(server_fd, str, sizeof(str));
    printf("Send data:[%s]\n", str);

    // 关闭套接字
    close(server_fd);

    return 0;
}
