#pragma once

#include <pthread.h>
#include <functional>

#include "../Locker.h"

#define NET_SRV_PORT            (5401)

typedef std::function<void(const char *, size_t)> fpi;

#define RegisterRecvHandle(srv, callback) \
                        srv.SetRecvHandle(std::bind(callback, this, std::placeholders::_1, std::placeholders::_2))

class NetServer
{
public:
    static NetServer &getInstance()
    {
        static NetServer instance;
        return instance;
    }

    int StartServer(int iSendBufferSize, int iRecvBufferSize);

    int SendMessage(const char *pData, size_t nSize);

    int SetRecvHandle(fpi fptr);

private:
    fpi m_RecvHandle;            // callback func

    int m_iSendBufferSize = 1024;
    int m_iRecvBufferSize = 1024;

    pthread_t m_ServerThreadID  = 0;
    int       m_iServerFD       = -1;
    bool      m_bStopServer     = false;
    bool      m_bRunning        = false;
    barrier   *m_pThreadBarrier = nullptr;
    pthread_t m_SendThreadID    = 0;
    int       m_iClientFD       = -1;
    bool      m_bStopClient     = false;
    char      *m_pSendBuffer    = nullptr;
    size_t    m_SendBufferLen   = 0;
    cond      m_condClientSend;

    NetServer() = default;

    ~NetServer();

    NetServer(const NetServer &);

    NetServer &operator=(const NetServer &);

    static void *ServerThread(void *arg);

    void ServerRoutine();

    int ClientRecvHandle();

    static void *SendThread(void *arg);

    void SendRoutine();
};
