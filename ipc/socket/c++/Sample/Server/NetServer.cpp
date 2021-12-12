#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/prctl.h>
#include "NetServer.h"
#include "../PrintUtil.h"
#include "CmdOptions.h"

NetServer::~NetServer()
{
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_YELLOW("NetServer::destructor\n");
    }

    m_bStopServer = true;
    m_bStopClient = true;
    if (m_iClientFD > 0)
    {
        shutdown(m_iClientFD, SHUT_RDWR);
        m_iClientFD = -1;
    }
    if (m_iServerFD > 0)
    {
        shutdown(m_iServerFD, SHUT_RDWR);
    }
    if (m_ServerThreadID)
    {
        pthread_join(m_ServerThreadID, nullptr);
    }

    free(m_pSendBuffer);

    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_YELLOW("NetServer::destructor done\n");
    }
}

int NetServer::StartServer(int iSendBufferSize, int iRecvBufferSize)
{
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_BLUE("NetServer::StartServer\n");
    }

    m_iSendBufferSize = iSendBufferSize;
    m_iRecvBufferSize = iRecvBufferSize;

    m_pSendBuffer = (char *) malloc(m_iSendBufferSize);

    m_bStopServer = false;
    if (m_bRunning)
    {
        FR_PRINT_GREEN("NetServer is already running\n");
        return 1;
    }

    m_pThreadBarrier = new barrier(2);

    if (pthread_create(&m_ServerThreadID, nullptr, ServerThread, this))
    {
        FR_PRINT_RED("Failed to create ServerThread\n");

        delete m_pThreadBarrier;
        m_pThreadBarrier = nullptr;
        return 1;
    }

    m_pThreadBarrier->wait();

    delete m_pThreadBarrier;
    m_pThreadBarrier = nullptr;

    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_BLUE("NetServer::StartServer done\n");
    }
    return 0;
}

void *NetServer::ServerThread(void *arg)
{
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_BLUE("NetServer::ServerThread\n");
    }
    NetServer *pNetServer = (NetServer *) arg;

    prctl(PR_SET_NAME, "NetServer");

    pNetServer->m_bRunning = true;

    pNetServer->m_pThreadBarrier->wait();

    pNetServer->ServerRoutine();

    pNetServer->m_bRunning = false;

    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_YELLOW("NetServer::ServerThread done\n");
    }
    pthread_exit(nullptr);
}

void NetServer::ServerRoutine()
{
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_BLUE("NetServer::ServerRoutine\n");
    }
    struct sockaddr_in server_addr;

    // 创建套接字
    if ((m_iServerFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("create socket failed\n");
        return;
    }

    // 将套接字和IP、端口绑定
    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family      = AF_INET;  //使用IPv4地址
    server_addr.sin_addr.s_addr = INADDR_ANY;  //具体的IP地址
    server_addr.sin_port        = htons(NET_SRV_PORT);  //端口

    // 设置套接字选项避免地址使用错误
    int on = 1;
    if ((setsockopt(m_iServerFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        FR_PRINT_RED("Failed to setsockopt\n");
        goto FAIL;
    }

    if ((bind(m_iServerFD, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1)
    {
        FR_PRINT_RED("Failed to bind socket on port %d\n", NET_SRV_PORT);
        goto FAIL;
    }

    // 进入监听状态，等待用户发起请求
    if ((listen(m_iServerFD, 20)) == -1)
    {
        FR_PRINT_RED("Failed to listen socket on port %d\n", NET_SRV_PORT);
        goto FAIL;
    }
    FR_PRINT_GREEN("NetServer listen on port %d\n", NET_SRV_PORT);

    while (!m_bStopServer)
    {
        // 接收客户端请求
        FR_PRINT_GREEN("NetServer wait for client app connect\n");
        if ((m_iClientFD = accept(m_iServerFD, nullptr, nullptr)) > 0)
        {
            FR_PRINT_GREEN("NetServer client app connected\n");

            m_bStopClient    = false;
            m_pThreadBarrier = new barrier(2);

            if (pthread_create(&m_SendThreadID, nullptr, SendThread, this))
            {
                FR_PRINT_RED("Failed to create SendThread\n");

                delete m_pThreadBarrier;
                m_pThreadBarrier = nullptr;
                close(m_iClientFD);
                continue;
            }

            m_pThreadBarrier->wait();

            delete m_pThreadBarrier;
            m_pThreadBarrier = nullptr;

            ClientRecvHandle();
        }
    }

FAIL:
    close(m_iServerFD);
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_YELLOW("NetServer::ServerRoutine done\n");
    }
}

int NetServer::ClientRecvHandle()
{
    int    iRet = 1;
    fd_set read_sd;
    FD_ZERO(&read_sd);
    FD_SET(m_iClientFD, &read_sd);

    while (true)
    {
        fd_set rsd = read_sd;

        int sel = select(m_iClientFD + 1, &rsd, nullptr, nullptr, nullptr);
        if (sel > 0)
        {
            // client has performed some activity (sent data or disconnected?)
            char arrBuffRead[2] = {0};

            int bytes = read(m_iClientFD, arrBuffRead, sizeof(arrBuffRead));
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
                // error receiving data from client. You may want to break from
                // while-loop here as well.
                perror("socket read");
                m_bStopClient = true;
                m_bStopServer = true;
                break;
            }
        }
        else if (sel < 0)
        {
            printf("grave error occurred\n");
            break;
        }
    }

    return iRet;
}

void *NetServer::SendThread(void *arg)
{
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_BLUE("NetServer::SendThread\n");
    }

    NetServer *pNetServer = (NetServer *) arg;

    prctl(PR_SET_NAME, "NetServerSend");

    pNetServer->m_pThreadBarrier->wait();

    pNetServer->SendRoutine();

    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_YELLOW("NetServer::SendThread done\n");
    }
    pthread_exit(nullptr);
}

void NetServer::SendRoutine()
{
    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_BLUE("NetServer::SendRoutine\n");
    }

    char   *pSendBuffer   = (char *) malloc(m_iSendBufferSize);
    size_t iSendBufferLen = 0;
    size_t len            = 0;

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

        len = write(m_iClientFD, pSendBuffer, iSendBufferLen);
//        printf("len:%zu\n", len);
        if (len < 0)
        {
            perror("send");
        }
    }

    free(pSendBuffer);

    if (G_AppOptions.nVerbose >= 3)
    {
        FR_PRINT_YELLOW("NetServer::SendRoutine done\n");
    }
}

int NetServer::SendMessage(const char *pData, size_t nSize)
{
    if (m_iClientFD <= 0 || m_bStopClient)
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

int NetServer::SetRecvHandle(fpi fptr)
{
    FR_PRINT_GREEN("NetServer SetRecvHandle\n");
    m_RecvHandle = fptr;
    return 0;
}
