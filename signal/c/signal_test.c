/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/6/29 22:43
 *
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sighandler(int signo, siginfo_t *si, void *data)
{
    if (si->si_signo == SIGHUP || si->si_signo == SIGQUIT)
    {
        printf("Ignore signal %d from pid %lu\n", (int) si->si_signo, (unsigned long) si->si_pid);
        return;
    }
    else
    {
        printf("Signal %d from pid %lu\n", (int) si->si_signo, (unsigned long) si->si_pid);
    }
    exit(0);
}

int main(void)
{
//    signal(SIGHUP, SIG_IGN);
//    signal(SIGQUIT, SIG_IGN);
//    signal(SIGTERM, SIG_IGN);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    sigaction(SIGHUP, &sa, 0);
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGQUIT, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    sigaction(SIGABRT, &sa, 0);
//    sigaction(SIGKILL, &sa, 0); SIGKILL是应用程序无法捕获的
    sigaction(SIGTERM, &sa, 0);
    printf("Pid %lu waiting for signal\n", (unsigned long) getpid());
    for (;;)
    {
        sleep(10);
    }
    return 0;
}
