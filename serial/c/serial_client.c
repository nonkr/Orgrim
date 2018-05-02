/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
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
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory.h>

#include "../../color.h"

extern HIST_ENTRY **history_list();

int sock_fd;

void SignalHandler(int signum)
{
    OGM_PRINT_CYAN("Interrupt signal (%d) received.\n", signum);
    close(sock_fd);
    exit(signum);
}

int main()
{
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGKILL, SignalHandler);

    //创建套接字
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family      = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = INADDR_ANY;  //具体的IP地址
    serv_addr.sin_port        = htons(4433);  //端口
    connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    char *temp = NULL;
    int done = 0;
    while (!done)
    {
        temp = readline("> ");

        /* Test for EOF. */
        if (!temp)
        {
            exit(1);
        }

        /* If there is anything on the line, print it and remember it. */
        if (*temp)
        {
            fprintf(stderr, "%s\n", temp);
            if (write(sock_fd, temp, strlen(temp)) < 0)
            {
                perror("write");
                return -1;
            }
            add_history(temp);
        }

        /* Check for `command' that we handle. */
        if (strcmp(temp, "quit") == 0)
        {
            done = 1;
        }
        else if (strcmp(temp, "list") == 0)
        {
            HIST_ENTRY **list;
            register int i;
            list = history_list();
            if (list)
            {
                for (i = 0; list[i]; i++)
                {
                    fprintf(stderr, "%d: %s\n", i, list[i]->line);
                }
            }
        }

        free(temp);
    }

//    while (1)
//    {
//        char   *line = NULL;
//        size_t size;
//        if (getline(&line, &size, stdin) == -1)
//        {
//            OGM_PRINT_ORANGE("no line\n");
//        }
//        else
//        {
//            line[strlen(line) - 1] = '\0';
//            if (!strcmp(line, "quit"))
//            {
//                break;
//            }
//            else
//            {
//                if (write(sock_fd, line, strlen(line)) < 0)
//                {
//                    perror("write");
//                    return -1;
//                }
//            }
//        }
//    }

    //关闭套接字
    close(sock_fd);

    return 0;
}
