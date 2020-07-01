#include <cstdio>
#include <csignal>
#include <unistd.h>
#include "CmdOptions.h"
#include "NetServer.h"
#include "ServerApp.h"

ServerApp *G_pApp = nullptr;

bool G_RUNNING = true;

void SignalHandler(int signum)
{
    G_RUNNING = false;
    if (G_pApp)
    {
        G_pApp->Stop();
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, SignalHandler);

    ParseCmdOptions(argc, argv);

    // 将程序运行在后台
    if (G_AppOptions.bBackground)
    {
        if (daemon(0, 1) == -1)
        {
            printf("daemon error\n");
        }
    }

    NetServer &pNetServer = NetServer::getInstance();

    G_pApp = new ServerApp;

    pNetServer.StartServer(1024, 1024);

    G_pApp->SendMessage();

    while (G_RUNNING)
    {
        sleep(1);
    }

    return 0;
}
