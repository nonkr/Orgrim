//
// Created by songbinbin on 2019/8/21.
//

#ifndef __NET_SERVER_APP_H__
#define __NET_SERVER_APP_H__

#include <cstdlib>

class ServerApp
{
public:
    ServerApp();

    ~ServerApp();

    void MessageRecvHandle(const char *pData, size_t nSize);

    void SendMessage();

    void Stop();

private:
    bool m_bStop = false;
};

#endif //__NET_SERVER_APP_H__
