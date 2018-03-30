/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/30 9:45
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define ERR_EXIT(m) \
do\
{\
    perror(m);\
    exit(EXIT_FAILURE);\
}\
while (0);\


void creat_daemon(void)
{
    pid_t pid;
    pid = fork();
    if (pid == -1)
        ERR_EXIT("fork error");
    if (pid > 0)
        exit(EXIT_SUCCESS);
    if (setsid() == -1)
        ERR_EXIT("SETSID ERROR");
    chdir("/");
    int i;
    for (i = 0; i < 3; ++i)
    {
        close(i);
        open("/dev/null", O_RDWR);
        dup(0);
        dup(0);
    }
    umask(0);
}

int main(void)
{
    time_t t;
    int    fd;

    creat_daemon();

    while (1)
    {
        fd = open("/tmp/daemon.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
            ERR_EXIT("open error");
        t = time(0);
        char *buf = asctime(localtime(&t));
        write(fd, buf, strlen(buf));
        close(fd);
        sleep(1);
    }

    return 0;
}
