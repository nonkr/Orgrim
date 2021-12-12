#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/prctl.h>
#include <linux/tcp.h>
#include "NetClient.h"
#include "../PrintUtil.h"
#include "CmdOptions.h"

int NetClient::Start()
{
    m_pSendBuffer = (char *) malloc(m_iSendBufferSize);

    if (pthread_create(&m_SendThreadID, nullptr, ConnectThread, this))
    {
        FR_PRINT_RED("Failed to create ConnectThread\n");
        return 1;
    }

    return 0;
}

void *NetClient::ConnectThread(void *arg)
{
    NetClient *pNetClient = (NetClient *) arg;
    FR_PRINT_BLUE("NetClient::ConnectThread\n");

    prctl(PR_SET_NAME, "SocketConnect");

    pNetClient->ConnectRoutine();

    FR_PRINT_YELLOW("NetClient::ConnectThread done\n");
    pthread_exit(nullptr);
}

void NetClient::ConnectRoutine()
{
    FR_PRINT_BLUE("NetClient::ConnectRoutine\n");
    int                len;
    struct sockaddr_in address;

    while (!m_bStop)
    {
        if ((m_iClientFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        {
            perror("sock");
            exit(1);
        }

        int syncnt = 1;
        setsockopt(m_iClientFD, IPPROTO_TCP, TCP_SYNCNT, &syncnt, sizeof(syncnt));

        memset(&address, 0x00, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port   = htons(G_AppOptions.nServerPort);
        inet_pton(AF_INET, G_AppOptions.pServerIP, &address.sin_addr);
        len = sizeof(address);

        FR_PRINT_ORANGE("try to connect %s\n", G_AppOptions.pServerIP);
        while ((connect(m_iClientFD, (struct sockaddr *) &address, len)) == -1)
        {
            perror("connect");
            if (m_bStop)
            {
                return;
            }
        }
        FR_PRINT_ORANGE("connected %s\n", G_AppOptions.pServerIP);
        m_bDisconnected = false;

        m_pThreadBarrier = new barrier(3);

        if (pthread_create(&m_SendThreadID, nullptr, SendThread, this))
        {
            FR_PRINT_RED("Failed to create SendThread\n");

            delete m_pThreadBarrier;
            m_pThreadBarrier = nullptr;
            close(m_iClientFD);
            return;
        }

        if (pthread_create(&m_RecvThreadID, nullptr, RecvThread, this))
        {
            FR_PRINT_RED("Failed to create RecvThread\n");

            delete m_pThreadBarrier;
            m_pThreadBarrier = nullptr;
            close(m_iClientFD);
            return;
        }

        m_pThreadBarrier->wait();
        delete m_pThreadBarrier;
        m_pThreadBarrier = nullptr;

        m_condReconnect.lock();
        while (!m_bDisconnected)
        {
            m_condReconnect.wait();
            if (m_bStop)
            {
                break;
            }
            else
            {
                sleep(1);
            }
        }
        m_condReconnect.unlock();

        if (m_iClientFD > 0)
        {
            close(m_iClientFD);
        }

        if (m_bStop) break;
    }

    FR_PRINT_YELLOW("NetClient::ConnectRoutine done\n");
}

void *NetClient::RecvThread(void *arg)
{
    NetClient *pNetClient = (NetClient *) arg;
    FR_PRINT_BLUE("NetClient::RecvThread\n");

    prctl(PR_SET_NAME, "SocketRecv");

    pNetClient->m_pThreadBarrier->wait();

    pNetClient->RecvRoutine();

    FR_PRINT_YELLOW("NetClient::RecvThread done\n");
    pthread_exit(nullptr);
}

void NetClient::RecvRoutine()
{
    FR_PRINT_BLUE("NetClient::RecvRoutine\n");
    fd_set read_sd;
    FD_ZERO(&read_sd);
    FD_SET(m_iClientFD, &read_sd);

    size_t         iReadSize            = 2;
    int            iSelectTimedoutCount = 0;
    struct timeval tv;

    while (!m_bStop)
    {
        fd_set rsd = read_sd;

        // ³¬Ê±ÉèÖÃ
        tv.tv_sec  = 3;
        tv.tv_usec = 0;

        int sel = select(m_iClientFD + 1, &rsd, nullptr, nullptr, &tv);
        if (sel > 0)
        {
            iSelectTimedoutCount = 0;

            // client has performed some activity (sent data or disconnected?)
            char arrBuffRead[1024] = {0}; // TODO: use decode to complete an whole package

            int bytes = recv(m_iClientFD, arrBuffRead, iReadSize, MSG_WAITALL);
            if (bytes > 0)
            {
                iReadSize = m_RecvHandle(arrBuffRead, bytes);
            }
            else if (bytes == 0)
            {
                FR_PRINT_ORANGE("client disconnected\n");
                m_condReconnect.lock();
                m_bDisconnected = true;
                m_condReconnect.signal();
                m_condReconnect.unlock();
                break;
            }
            else
            {
                perror("socket read");
                // error receiving data from client. You may want to break from
                // while-loop here as well.
            }
        }
        else if (sel == 0)
        {
            iSelectTimedoutCount++;
            FR_PRINT_RED("[%s] connection timed out:%d\n", TimeUtil::TimeNowString().c_str(), iSelectTimedoutCount);
            if (iSelectTimedoutCount > 10)
            {
                m_condReconnect.lock();
                m_bDisconnected = true;
                m_condReconnect.signal();
                m_condReconnect.unlock();
                break;
            }
        }
        else if (sel < 0)
        {
            FR_PRINT_RED("grave error occurred\n");
            break;
        }
    }

    FR_PRINT_YELLOW("NetClient::RecvRoutine done\n");
}

void NetClient::Stop()
{
    FR_PRINT_YELLOW("NetClient::Stop\n");

    m_bStop = true;
    if (m_iClientFD > 0)
    {
        shutdown(m_iClientFD, SHUT_RDWR);
        m_iClientFD = 0;
    }

    if (m_RecvThreadID)
    {
        pthread_join(m_RecvThreadID, nullptr);
        m_RecvThreadID = 0;
    }

    free(m_pSendBuffer);

    FR_PRINT_YELLOW("NetClient::Stop done\n");
}

int NetClient::SetRecvHandle(fpi fptr)
{
    FR_PRINT_GREEN("NetClient SetRecvHandle\n");
    m_RecvHandle = fptr;
    return 0;
}

void *NetClient::SendThread(void *arg)
{
    FR_PRINT_BLUE("NetServer::SendThread\n");

    NetClient *pNetClient = (NetClient *) arg;

    prctl(PR_SET_NAME, "NetServerSend");

    pNetClient->m_pThreadBarrier->wait();

    pNetClient->SendRoutine();

    FR_PRINT_YELLOW("NetServer::SendThread done\n");
    pthread_exit(nullptr);
}

void NetClient::SendRoutine()
{
    FR_PRINT_BLUE("NetServer::SendRoutine\n");

    char   *pSendBuffer   = (char *) malloc(m_iSendBufferSize);
    size_t iSendBufferLen = 0;

    while (!m_bDisconnected && m_iClientFD > 0)
    {
        m_condClientSend.lock();
        while (m_SendBufferLen <= 0)
        {
            m_condClientSend.wait();
        }
        memcpy(pSendBuffer, m_pSendBuffer, m_SendBufferLen);
        iSendBufferLen  = m_SendBufferLen;
        m_SendBufferLen = 0;
        m_condClientSend.unlock();

        int i = write(m_iClientFD, pSendBuffer, iSendBufferLen);
    }

    free(pSendBuffer);

    FR_PRINT_YELLOW("NetServer::SendRoutine done\n");
}

int NetClient::SendMessage(char *pData, size_t nSize)
{
    if (m_iClientFD <= 0)
    {
        FR_PRINT_ORANGE("no client connected, can not send message\n");
        return 1;
    }

    m_condClientSend.lock();
    if (m_SendBufferLen != 0)
    {
        m_condClientSend.unlock();
        return 1;
    }
    memcpy(m_pSendBuffer + m_SendBufferLen, pData, nSize);
    m_SendBufferLen += nSize;
    m_condClientSend.signal();
    m_condClientSend.unlock();
    return 0;
}
