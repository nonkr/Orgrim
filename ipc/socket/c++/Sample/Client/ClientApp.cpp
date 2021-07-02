
#include <cstdlib>
#include <cstring>
#include "ClientApp.h"
#include "NetClient.h"
#include "../PrintUtil.h"

ClientApp::ClientApp()
{
    m_pNetClient = new NetClient;
    RegisterRecvHandle(m_pNetClient, &ClientApp::MessageRecvHandle);
    m_pNetClient->Start();
}

ClientApp::~ClientApp()
{
    m_pNetClient->Stop();
}

void ClientApp::MessageRecvHandle(const char *pData, size_t nSize)
{
    FR_PRINT_YELLOW("MessageRecvHandle, [%zu][%s]\n", nSize, pData);

    char *str = (char *) "ok";
    m_pNetClient->SendMessage(str, strlen(str));
}
