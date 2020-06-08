#include <csignal>
#include <bits/siginfo.h>
#include "PingUtil.h"

void signalHandler(int signo, siginfo_t *si, void *data)
{
    if (si->si_signo == SIGHUP || si->si_signo == SIGQUIT || si->si_signo == SIGTSTP)
    {
//        printf("Ignore interrupt signal (%d) received from pid %lu, main pid %d\n",
//               si->si_signo, (unsigned long) si->si_pid, getpid());
        return;
    }
    else
    {
//        printf("Interrupt signal (%d) received from pid %lu, main pid %d\n",
//               si->si_signo, (unsigned long) si->si_pid, getpid());
    }

    PingUtil::getInstance().Quit();

    exit(si->si_signo);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./ping <ip or hostname>\n");
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = signalHandler;

    sigaction(SIGHUP, &sa, 0);
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGQUIT, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    sigaction(SIGABRT, &sa, 0);
//    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGTERM, &sa, 0);
    sigaction(SIGSTOP, &sa, 0);
    sigaction(SIGTSTP, &sa, 0);

    PingUtil::getInstance().Init(argv[1]);

    while (true)
    {
        sleep(UINT32_MAX);
    }
}
