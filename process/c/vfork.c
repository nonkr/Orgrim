/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    pid_t pid;
    int   cnt = 0;
    pid = vfork();
    if (pid < 0)
    {
        printf("error in fork!\n");
    }
    else if (pid == 0)
    {
        cnt++;
        printf("cnt=%d\n", cnt);
        printf("I am the child process,ID is %d\n", getpid());

        // 结束子进程不用exit(0)，而使用_exit(0)。这是因为_exit(0)在结束进程时，不对标准I/O流进行任何操作。而exit(0)则会关闭进程的所有标准I/O流。
        _exit(0);
    }
    else
    {
        cnt++;
        printf("cnt=%d\n", cnt);
        printf("I am the parent process,ID is %d\n", getpid());
    }
    return 0;
}