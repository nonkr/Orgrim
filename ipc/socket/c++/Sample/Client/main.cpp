#include <cstdio>
#include <csignal>
#include <unistd.h>
#include "CmdOptions.h"
#include "NetClient.h"
#include "ClientApp.h"

bool G_RUNNING = true;

void SignalHandler(int signum)
{
    G_RUNNING = false;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, SignalHandler);

    ParseCmdOptions(argc, argv);

    if (G_AppOptions.pServerIP == nullptr)
    {
        printf("Need server ip\n");
        return 1;
    }

    // 将程序运行在后台
    if (G_AppOptions.bBackground)
    {
        if (daemon(0, 1) == -1)
        {
            printf("daemon error\n");
        }
    }

    ClientApp *pApp = new ClientApp;

    while (G_RUNNING)
    {
        sleep(1);
    }

    delete pApp;

    return 0;
}
