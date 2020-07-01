#pragma once

#include <cstdio>
#include <ctime>
#include <netdb.h>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <csignal>
#include <cmath>
#include <string>
#include <pthread.h>
#include <chrono>

#define ICMP_DATA_LEN 56        //ICMP默认数据长度
#define ICMP_HEAD_LEN 8            //ICMP默认头部长度
#define ICMP_LEN  (ICMP_DATA_LEN + ICMP_HEAD_LEN)
#define SEND_BUFFER_SIZE 128        //发送缓冲区大小
#define RECV_BUFFER_SIZE 128        //接收缓冲区大小
#define SEND_NUM 100            //发送报文数
#define MAX_WAIT_TIME 3

using namespace std;
using namespace chrono;

class PingUtil
{
public:
    static PingUtil &getInstance()
    {
        static PingUtil instance;
        return instance;
    }

    int Init(char *pIP);

    void Quit();

//发送ICMP报文
    void SendPacket(int sock_icmp, struct sockaddr_in *dest_addr, int nSend);

//接收ICMP报文
    int RecvePacket(int sock_icmp, struct sockaddr_in *dest_addr);

//计算校验和
    u_int16_t Compute_cksum(struct icmp *pIcmp);

//设置ICMP报文
    void SetICMP(u_int16_t seq);

//剥去报头
    int unpack(struct timeval *RecvTime);

//计算往返时间
    double GetRtt(struct timeval *RecvTime, struct timeval *SendTime);

//统计信息
    void Statistics();

    PingUtil &operator=(const PingUtil &);

private:
    int                socket_fd                    = 0;
    struct sockaddr_in dest_addr;    //IPv4专用socket地址,保存目的地址
    struct hostent     *pHost;
    char               *IP;
    char               SendBuffer[SEND_BUFFER_SIZE] = {0};
    char               RecvBuffer[RECV_BUFFER_SIZE] = {0};
    int                nRecv                        = 0;    //实际接收到的报文数
    struct timeval     FirstSendTime;    //用以计算总的时间
    struct timeval     LastRecvTime;
    double             min                          = 0.0;
    double             avg                          = 0.0;
    double             max                          = 0.0;
    double             mdev                         = 0.0;
    pthread_t          m_ThreadID                   = 0;
    int                nSend                        = 1;
    bool               m_bStop                      = false;

    PingUtil() = default;;

    ~PingUtil() = default;

    PingUtil(const PingUtil &);

    void PingRoutine();

    static void *PingThread(void *arg);

    static std::string TimeNowString();
};
