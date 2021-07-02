#pragma once

#include <cstdlib>
#include "NetClient.h"

class ClientApp
{
public:
    ClientApp();

    ~ClientApp();

    void MessageRecvHandle(const char *pData, size_t nSize);

private:
    NetClient *m_pNetClient = nullptr;
};
