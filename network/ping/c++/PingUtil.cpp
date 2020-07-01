#include <sys/prctl.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>

#include "PingUtil.h"

void PingUtil::Quit()
{
    m_bStop = true;

    if (m_ThreadID)
    {
        pthread_join(m_ThreadID, nullptr);
    }
}

int PingUtil::Init(char *pIP)
{
    in_addr_t      inaddr;        //ip地址（网络字节序）
    struct timeval tv_out;
    tv_out.tv_sec  = 1;
    tv_out.tv_usec = 0;

    IP = pIP;

    /* 创建ICMP套接字 */
    //AF_INET:IPv4, SOCK_RAW:IP协议数据报接口, IPPROTO_ICMP:ICMP协议
    if ((socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    dest_addr.sin_family = AF_INET;

    /* 将点分十进制ip地址转换为网络字节序 */
    if ((inaddr = inet_addr(pIP)) == INADDR_NONE)
    {
        /* 转换失败，表明是主机名,需通过主机名获取ip */
        if ((pHost = gethostbyname(pIP)) == nullptr)
        {
            herror("gethostbyname()");
            exit(EXIT_FAILURE);
        }
        memmove(&dest_addr.sin_addr, pHost->h_addr_list[0], pHost->h_length);
    }
    else
    {
        memmove(&dest_addr.sin_addr, &inaddr, sizeof(struct in_addr));
    }

    if (nullptr != pHost)
        printf("PING %s", pHost->h_name);
    else
        printf("PING %s", pIP);
    printf("(%s) %d bytes of data.\n", inet_ntoa(dest_addr.sin_addr), ICMP_LEN);

    // setting timeout of recv setting
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv_out, sizeof tv_out);

    if (pthread_create(&m_ThreadID, nullptr, PingThread, this))
    {
        printf("Failed to create PingThread\n");
    }

    return 0;
}

u_int16_t PingUtil::Compute_cksum(struct icmp *pIcmp)
{
    u_int16_t *data = (u_int16_t *) pIcmp;
    int       len   = ICMP_LEN;
    u_int32_t sum   = 0;

    while (len > 1)
    {
        sum += *data++;
        len -= 2;
    }
    if (1 == len)
    {
        u_int16_t tmp = *data;
        tmp &= 0xff00;
        sum += tmp;
    }

    //ICMP校验和带进位
    while (sum >> 16)
        sum = (sum >> 16) + (sum & 0x0000ffff);

    sum = ~sum;

    return sum;
}

void PingUtil::SetICMP(u_int16_t seq)
{
    struct icmp    *pIcmp;
    struct timeval *pTime;

    pIcmp = (struct icmp *) SendBuffer;

    /* 类型和代码分别为ICMP_ECHO,0代表请求回送 */
    pIcmp->icmp_type  = ICMP_ECHO;
    pIcmp->icmp_code  = 0;
    pIcmp->icmp_cksum = 0;        //校验和
    pIcmp->icmp_seq   = seq;        //序号
    pIcmp->icmp_id    = getpid();    //取进程号作为标志
    pTime = (struct timeval *) pIcmp->icmp_data;
    gettimeofday(pTime, nullptr);    //数据段存放发送时间
    pIcmp->icmp_cksum = Compute_cksum(pIcmp);

    if (1 == seq)
        FirstSendTime = *pTime;
}

void PingUtil::SendPacket(int sock_icmp, struct sockaddr_in *dest_addr, int nSend)
{
    SetICMP(nSend);
    if (sendto(sock_icmp, SendBuffer, ICMP_LEN, 0, (struct sockaddr *) dest_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("sendto");
        return;
    }
}

double PingUtil::GetRtt(struct timeval *RecvTime, struct timeval *SendTime)
{
    struct timeval sub = *RecvTime;

    if ((sub.tv_usec -= SendTime->tv_usec) < 0)
    {
        --(sub.tv_sec);
        sub.tv_usec += 1000000;
    }
    sub.tv_sec -= SendTime->tv_sec;

    return sub.tv_sec * 1000.0 + sub.tv_usec / 1000.0; //转换单位为毫秒
}

int PingUtil::unpack(struct timeval *RecvTime)
{
    struct ip   *Ip = (struct ip *) RecvBuffer;
    struct icmp *Icmp;
    int         ipHeadLen;
    double      rtt;

    ipHeadLen = Ip->ip_hl << 2;    //ip_hl字段单位为4字节
    Icmp      = (struct icmp *) (RecvBuffer + ipHeadLen);

    //判断接收到的报文是否是自己所发报文的响应
    if ((Icmp->icmp_type == ICMP_ECHOREPLY) && Icmp->icmp_id == getpid())
    {
        struct timeval *SendTime = (struct timeval *) Icmp->icmp_data;
        rtt = GetRtt(RecvTime, SendTime);

        printf("[%s] %u bytes from %s: icmp_seq=%u ttl=%u time=%.1f ms\n", TimeNowString().c_str(),
               ntohs(Ip->ip_len) - ipHeadLen,
               inet_ntoa(Ip->ip_src), Icmp->icmp_seq, Ip->ip_ttl, rtt);

        if (rtt < min || 0 == min)
            min = rtt;
        if (rtt > max)
            max = rtt;
        avg += rtt;
        mdev += rtt * rtt;
        return 0;
    }
    return -1;
}

void PingUtil::Statistics()
{
    double tmp;
    avg /= nRecv;
    tmp  = mdev / nRecv - avg * avg;
    mdev = sqrt(tmp);

    if (nullptr != pHost)
        printf("--- %s  ping statistics ---\n", pHost->h_name);
    else
        printf("--- %s  ping statistics ---\n", IP);

    printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n", nSend, nRecv,
           (nSend - nRecv) / nSend * 100, (int) GetRtt(&LastRecvTime, &FirstSendTime));
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
           min, avg, max, mdev);

//    close(sock_icmp);
//    exit(0);
}

int PingUtil::RecvePacket(int sock_icmp, struct sockaddr_in *dest_addr)
{
    int            RecvBytes = 0;
    socklen_t      addrlen   = sizeof(struct sockaddr_in);
    struct timeval RecvTime;

    if ((RecvBytes = recvfrom(sock_icmp, RecvBuffer, RECV_BUFFER_SIZE, 0, (struct sockaddr *) dest_addr, &addrlen)) < 0)
    {
        printf("[%s] recvfrom: %s\n", TimeNowString().c_str(), strerror(errno));
        return -2;
    }
    //printf("nRecv=%d\n", RecvBytes);
    gettimeofday(&RecvTime, nullptr);
    LastRecvTime = RecvTime;

    if (unpack(&RecvTime) == -1)
    {
        return -1;
    }
    nRecv++;

    return 0;
}

void PingUtil::PingRoutine()
{
    struct sockaddr_in cp_dest_addr;

    while (!m_bStop)
    {
        int unpack_ret;

        SendPacket(socket_fd, &dest_addr, nSend);
        memcpy(&cp_dest_addr, &dest_addr, sizeof(struct sockaddr_in));
        unpack_ret = RecvePacket(socket_fd, &cp_dest_addr);

        if (-2 == unpack_ret)
        {
            nSend++;
            continue;
        }
        else if (-1 == unpack_ret)    //（ping回环时）收到了自己发出的报文,重新等待接收
        {
            if (RecvePacket(socket_fd, &dest_addr) == -2)
            {
                nSend++;
                continue;
            }
        }

        sleep(1);
        nSend++;
    }

    Statistics();
}

void *PingUtil::PingThread(void *arg)
{
    PingUtil *pPingUtil = (PingUtil *) arg;

    prctl(PR_SET_NAME, "Ping");

    pPingUtil->PingRoutine();

    pthread_exit(nullptr);
}

std::string PingUtil::TimeNowString()
{
    auto tp = system_clock::now();

    auto ttime_t = system_clock::to_time_t(tp);

    std::tm *ttm = localtime(&ttime_t);

    char time_str[] = "yyyy-mm-dd HH:MM:SS";

    strftime(time_str, sizeof(time_str), "%F %T", ttm);

    string result(time_str);

    return result;
}
