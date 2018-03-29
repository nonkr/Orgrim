/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/2/26 15:15
 *
 */

#include <iostream>
#include <ev.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace std;

#define PORT 9999
#define BUFFER_SIZE 1024

// accept,事件的回调
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

// read,事件的回调
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

// 上帝在此
int main()
{
    /* ev_default_loop,初始化一个不是线程安全的loop
       ev_loop_new,初始化一个线程安全但不能捕捉信号和子进程的监视 */
    struct ev_loop *loop = ev_default_loop(0);

    // STDIN_FILENO
    int sd;

    // IO,事件
    struct ev_io socket_accept;

    // ----------------------------------------------- //

    // 地址结构,socket
    struct sockaddr_in addr;

    // 建立,socekt
    if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error");
        return -1;
    }

    // ----------------------------------------------- //

    // 置零
    memset(&addr, 0, sizeof(addr));

    // 地址族
    addr.sin_family = AF_INET;

    /* 必须要采用网络数据格式，普通数字可以用htons()函数转换成网络数据格式的数字 */
    addr.sin_port = htons(PORT);

    /* IP,地址结构
       INADDR_ANY,就是指定地址为0.0.0.0的地址 */
    addr.sin_addr.s_addr = INADDR_ANY;

    // ----------------------------------------------- //

    // 绑定
    if (bind(sd, (struct sockaddr *) &addr, sizeof(addr)) != 0)
    {
        printf("bind error");
        return -1;
    }

    // 建立监听，及设置请求连接队列的最大长度，用于限制排队请求的个数
    if (listen(sd, 5) < 0)
    {
        printf("listen error");
        return -1;
    }

    // ----------------------------------------------- /

    // 开始监听io事件
    ev_io_init(&socket_accept, accept_cb, sd, EV_IO);
    ev_io_start(loop, &socket_accept);

    // 循环
    while (true)
    {
        // 开始事件循环
        ev_run(loop, 0);
    }

    // 返回
    return 0;
}

// accept 事件的回调
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    // 地址结构,socket
    struct sockaddr_in client_addr;

    // 地址结构长度,socket
    socklen_t client_len = sizeof(client_addr);

    // STDIN_FILENO
    int client_sd;

    // 分派客户端的ev_io结构
    auto *w_client = new(ev_io);

    // libev,错误处理
    if (EV_ERROR & revents)
    {
        printf("error event in accept");
        return;
    }

    // accept,普通写法
    client_sd = accept(watcher->fd, (struct sockaddr *) &client_addr, &client_len);
    if (client_sd < 0)
    {
        printf("accept error");
        return;
    }

    printf("someone connected.\n");

    // 开始监听读事件了,有客户端信息就会被监听到
    ev_io_init(w_client, read_cb, client_sd, EV_IO);
    ev_io_start(loop, w_client);
}

// read 事件的回调
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    // 缓冲区
    char buffer[BUFFER_SIZE];

    // 读取到的字节数
    ssize_t read;

    // libev,错误处理
    if (EV_ERROR & revents)
    {
        printf("error event in read");
        return;
    }

    while (true)
    {
        // 最后记得置零
        memset(buffer, 0, BUFFER_SIZE);

        // 读取本次输入数据
        read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

        cout << buffer << endl;

        // 原信息返回,也可以自己写信息,都一样.
        send(watcher->fd, buffer, read, 0);
    }
}
