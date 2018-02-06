/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/16 23:00
 *
 */

#include <sys/select.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;

    // 创建有名管道
    if (mkfifo("/tmp/test_fifo", 0666) != 0)
    {
        perror("mkfifo：");
    }

    // 读写方式打开管道
    fd = open("/tmp/test_fifo", O_RDWR);
    if (fd < 0)
    {
        perror("open fifo");
        return -1;
    }

    while (1)
    {
        char *str = "this is for test";
        write(fd, str, strlen(str)); // 往管道里写内容
        printf("after write to fifo\n");
        sleep(3);
    }

    return 0;
}
