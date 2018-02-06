/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/16 22:53
 *
 */

#include <sys/select.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    fd_set rfds;
    struct timeval tv;
    int ret;
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
        // 这部分内容，要放在while(1)里面
        FD_ZERO(&rfds);     // 清空
        FD_SET(STDIN_FILENO, &rfds);   // 标准输入描述符 0 加入集合
        FD_SET(fd, &rfds);  // 有名管道描述符 fd 加入集合

        // 超时设置
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        // 监视并等待多个文件（标准输入，有名管道）描述符的属性变化（是否可读）
        // 没有属性变化，这个函数会阻塞，直到有变化才往下执行
        // FD_SETSIZE 为 <sys/select.h> 的宏定义，默认为1024
//        ret = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);
        ret = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);

        if (ret == -1)
        {   // 出错
            perror("select()");
        }
        else if (ret > 0)
        {   // 准备就绪的文件描述符

            char buf[100] = {0};
            if (FD_ISSET(0, &rfds))
            { // 标准输入
                read(0, buf, sizeof(buf));
                printf("stdin buf = [%s]\n", buf);

            }
            else if (FD_ISSET(fd, &rfds))
            { // 有名管道
                read(fd, buf, sizeof(buf));
                printf("fifo buf = [%s]\n", buf);
            }

        }
        else if (ret == 0)
        {   // 超时
            printf("time out\n");
        }
    }

    return 0;
}
