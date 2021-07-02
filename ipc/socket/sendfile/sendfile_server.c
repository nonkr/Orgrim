/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/3 22:11
 *
 */

/***************************************************
* 文件名：pthread_server.c
* 文件描述：创建子线程来接收客户端的数据
***************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>

void *rec_data(void *fd);

int main(int argc, char *argv[])
{
    int                server_fd;
    int                *client_fd;
    int                client_len;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    // 创建套接字
    if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("create socket failed\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(9734);

    // 设置套接字选项避免地址使用错误
    int on = 1;
    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字
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

    printf("server waiting for connect\n");
    while (1)
    {
        pthread_t thread; // 创建不同的子线程以区别不同的客户端
        client_fd  = (int *) malloc(sizeof(int));
        client_len = sizeof(client_addr);
        *client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
        if (-1 == *client_fd)
        {
            perror("accept");
            continue;
        }
        if (pthread_create(&thread, NULL, rec_data, client_fd))//创建子线程
        {
            perror("pthread_create");
            break;
        }
    }
    shutdown(*client_fd, SHUT_RDWR);
    shutdown(server_fd, SHUT_RDWR);
}

/*****************************************
* 函数名称：rec_data
* 功能描述：接受客户端的数据
* 参数列表：fd——连接套接字
* 返回结果：void
*****************************************/
void *rec_data(void *fd)
{
    int    client_fd = *((int *) fd);
    fd_set read_sd;
    FD_ZERO(&read_sd);
    FD_SET(client_fd, &read_sd);

    printf("client %d connected\n", client_fd);
    while (1)
    {
        fd_set rsd = read_sd;

        int sel = select(client_fd + 1, &rsd, 0, 0, 0);
        if (sel > 0)
        {
            // client has performed some activity (sent data or disconnected?)
            char buf[1024] = {0};

            int bytes = read(client_fd, buf, sizeof(buf));
            if (bytes > 0)
            {
                printf("Message form client: %s\n", buf);
                if (strcmp(buf, "exit") == 0)//接受到exit时，跳出循环
                    break;
            }
            else if (bytes == 0)
            {
                printf("client %d disconnected\n", client_fd);
                break;
            }
            else
            {
                printf("error read\n");
                // error receiving data from client. You may want to break from
                // while-loop here as well.
                break;
            }
        }
        else if (sel < 0)
        {
            printf("grave error occurred\n");
            break;
        }
    }

    close(client_fd);
    free(fd);
    pthread_exit(NULL);
}
