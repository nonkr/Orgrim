#pragma once

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
