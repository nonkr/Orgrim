#pragma once

#include <pthread.h>
#include <functional>
#include "../Locker.h"

typedef std::function<int(const char *, size_t)> fpi;

#define RegisterRecvHandle(srv, callback) \
                        srv->SetRecvHandle(std::bind(callback, this, std::placeholders::_1, std::placeholders::_2))

class NetClient
{
public:
    NetClient() = default;

    ~NetClient() = default;

    int Start();

    void Stop();

    int SetRecvHandle(fpi fptr);

    int SendMessage(char *pData, size_t nSize);

private:
    fpi m_RecvHandle;            // callback func

    int m_iSendBufferSize = 1024;
    int m_iRecvBufferSize = 1024;

    int       m_iClientFD       = 0;
    bool      m_bStop           = false;
    pthread_t m_SendThreadID    = 0;
    pthread_t m_RecvThreadID    = 0;
    barrier   *m_pThreadBarrier = nullptr;

    char   *m_pSendBuffer  = nullptr;
    size_t m_SendBufferLen = 0;
    cond   m_condClientSend;

    bool m_bDisconnected = true;
    cond m_condReconnect;

    static void *ConnectThread(void *arg);

    void ConnectRoutine();

    static void *RecvThread(void *arg);

    void RecvRoutine();

    static void *SendThread(void *arg);

    void SendRoutine();
};
