#ifndef __NETCLIENT_H__
#define __NETCLIENT_H__

#include <pthread.h>
#include <functional>
#include "../Locker.h"

#define TR_SERVER_PORT    (5401)

typedef std::function<void(const char *, size_t)> fpi;

#define SubscribeRecvHandle(srv, callback) \
                        srv->SetRecvHandle(std::bind(callback, this, std::placeholders::_1, std::placeholders::_2));

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

    bool      m_bStopClient     = false;
    int       m_iClientFD       = 0;
    bool      m_bStop           = false;
    pthread_t m_SendThreadID    = 0;
    pthread_t m_RecvThreadID    = 0;
    barrier   *m_pThreadBarrier = nullptr;

    char   *m_pSendBuffer  = nullptr;
    size_t m_SendBufferLen = 0;
    cond   m_condClientSend;

    static void *RecvThread(void *arg);

    void RecvRoutine();

    static void *SendThread(void *arg);

    void SendRoutine();
};

#endif // __NETCLIENT_H__
