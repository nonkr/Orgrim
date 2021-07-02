//
// Created by songbinbin on 2019/8/21.
//

#include <cstdlib>
#include <cstring>
#include "ServerApp.h"
#include "NetServer.h"
#include "../PrintUtil.h"

ServerApp::ServerApp()
{
    NetServer &pNetServer = NetServer::getInstance();
    RegisterRecvHandle(pNetServer, &ServerApp::MessageRecvHandle);
}

ServerApp::~ServerApp()
{
    NetServer &pNetServer = NetServer::getInstance();
}

void ServerApp::MessageRecvHandle(const char *pData, size_t nSize)
{
    FR_PRINT_YELLOW("MessageRecvHandle, [%zu][%s]\n", nSize, pData);
}

void ServerApp::SendMessage()
{
    NetServer &pNetServer = NetServer::getInstance();

    char *str = (char *) "hello";

    while (!m_bStop)
    {
        pNetServer.SendMessage(str, strlen(str));

        sleep(1);
    }
}

void ServerApp::Stop()
{
    m_bStop = true;
}
