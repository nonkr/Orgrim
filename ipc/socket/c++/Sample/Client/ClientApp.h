//
// Created by songbinbin on 2019/8/21.
//

#ifndef __NET_SERVER_APP_H__
#define __NET_SERVER_APP_H__

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

#endif //__NET_SERVER_APP_H__
