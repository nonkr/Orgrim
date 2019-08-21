#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/prctl.h>
#include "NetClient.h"
#include "../PrintUtil.h"

int NetClient::Start()
{
    int                len;
    struct sockaddr_in address;

    m_pSendBuffer = (char *) malloc(m_iSendBufferSize);

    if ((m_iClientFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("sock");
        exit(1);
    }
    memset(&address, 0x00, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port   = htons(TR_SERVER_PORT);
    inet_pton(AF_INET, TR_SERVER_IP_ADDR, &address.sin_addr);
    len = sizeof(address);

    FR_PRINT_ORANGE("try to connect %s\n", TR_SERVER_IP_ADDR);
    if ((connect(m_iClientFD, (struct sockaddr *) &address, len)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    FR_PRINT_ORANGE("connected %s\n", TR_SERVER_IP_ADDR);

    m_pThreadBarrier = new barrier(3);

    if (pthread_create(&m_SendThreadID, nullptr, SendThread, this))
    {
        FR_PRINT_RED("Failed to create SendThread\n");

        delete m_pThreadBarrier;
        m_pThreadBarrier = nullptr;
        close(m_iClientFD);
        return 1;
    }

    if (pthread_create(&m_RecvThreadID, nullptr, RecvThread, this))
    {
        FR_PRINT_RED("Failed to create RecvThread\n");

        delete m_pThreadBarrier;
        m_pThreadBarrier = nullptr;
        close(m_iClientFD);
        return 1;
    }

    m_pThreadBarrier->wait();
    delete m_pThreadBarrier;
    m_pThreadBarrier = nullptr;

    return 0;
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

    while (!m_bStop)
    {
        fd_set rsd = read_sd;

        int sel = select(m_iClientFD + 1, &rsd, nullptr, nullptr, nullptr);
        if (sel > 0)
        {
            // client has performed some activity (sent data or disconnected?)
            char arrBuffRead[5] = {0}; // TODO: use decode to complete an whole package

            int bytes = recv(m_iClientFD, arrBuffRead, sizeof(arrBuffRead), MSG_WAITALL);
            if (bytes > 0)
            {
                m_RecvHandle(arrBuffRead, bytes);
            }
            else if (bytes == 0)
            {
                FR_PRINT_ORANGE("client disconnected\n");
                m_bStopClient = true;
                break;
            }
            else
            {
                perror("socket read");
                // error receiving data from client. You may want to break from
                // while-loop here as well.
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

    while (!m_bStopClient && m_iClientFD > 0)
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
