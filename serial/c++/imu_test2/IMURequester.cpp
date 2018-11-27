#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/prctl.h>
#include <errno.h>
#include <cinttypes>
#include <poll.h>

#include "serial.h"
#include "IMURequester.h"
#include "PrintUtil.h"
#include "DebugUtil.h"
#include "TimeUtil.h"

static const int PacketHeader = 0xAA;
static const int MAX_LEN      = 1680;  //缓冲区最大长度

struct EvIMUReplyData_t
{
    unsigned char  ucID;                            // Message ID
    unsigned short usX;                             // Location's X-axis of Ev, in 15cm resolution
    unsigned short usY;                             // Location's Y-axis of Ev, in 15cm resolution
    char           cType;                           // Location's type of Ev
    unsigned short usDegrees;                       // Location's degrees of Ev
    unsigned short usDust;                          // Location's dust info of Ev
    unsigned int   uiLeftOdometer;                  // left odometer
    unsigned int   uiRightOdometer;                 // right odometer
    float          fOriginalX;                      // Location's X-axis of Ev, original in 1m
    float          fOriginalY;                      // Location's Y-axis of Ev, original in 1m
    unsigned short usRightToFDistance;              // distance of right single tof(mm)
    unsigned short usLeftToFDistance;               // distance of left single tof(mm)
    unsigned char  kMotionDirection;                // motion direction
}  __attribute__((__packed__));

IMURequester::IMURequester()
{
    m_bReceivedIMUData   = false;
    m_nUsartfd           = -1;
    m_TotalRequestCount  = 0;
    m_TotalTimedoutCount = 0;

    SerialOpts stSerialOpts = {
        .pDevice   = (char *) "/dev/ttyS1",
        .nSpeed    = 460800,
        .nDatabits = 8,
        .nStopbits = 1,
        .nParity   = 'n',
    };

    if ((m_nUsartfd = open_serial(&stSerialOpts)) < 0)
    {
        fprintf(stderr, "open serial failed\n");
        return;
    }

    pthread_mutex_init(&m_ReceivedMutex, nullptr);
    pthread_cond_init(&m_ReceivedCond, nullptr);
    pthread_barrier_init(&m_ThreadIsRunning, nullptr, 2);

    if (pthread_create(&m_RxID, nullptr, RecvThread, this))
    {
        printf("pthread_create RecvThread failed\n");
        return;
    }

    unlink("/tmp/imu.csv");
    fp = fopen("/tmp/imu.csv", "w");

    pthread_barrier_wait(&m_ThreadIsRunning);
}

IMURequester::~IMURequester()
{
    pthread_cancel(m_RxID);
    pthread_join(m_RxID, nullptr);
    pthread_barrier_destroy(&m_ThreadIsRunning);
    fclose(fp);
}

inline static float FloatSwap(float value)
{
    union v
    {
        float        f;
        unsigned int i;
    };

    union v val;

    val.f = value;
    val.i = htonl(val.i);

    return val.f;
};

int IMURequester::hexstring_to_bytearray(char *p_hexstring, char **pp_out, int *p_i_out_len)
{
    size_t i;
    size_t i_str_len;
    char   *p_out    = nullptr;
    int    i_out_len = 0;
    int    i_offset  = 0;

    if (p_hexstring == nullptr)
        return -1;

    i_str_len = strlen(p_hexstring);

    // 先初步对传入的字符串做一遍检查
    if (i_str_len == 2)
        i_out_len = 1;
    else if ((i_str_len - 2) % 3 == 0)
        i_out_len = (i_str_len - 2) / 3 + 1;
    else
    {
        fprintf(stderr, "malformed input hex string\n");
        goto error;
    }

    p_out = (char *) malloc((size_t) i_out_len);
    if (p_out == nullptr)
        return -1;

    for (i = 0; i < i_str_len;)
    {
        if (isxdigit(p_hexstring[i]) == 0 || isxdigit(p_hexstring[i + 1]) == 0)
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        sscanf(p_hexstring + i, "%2x", (unsigned int *) &(p_out[i_offset++]));

        if (i + 2 == i_str_len)
            break;

        if (p_hexstring[i + 2] != ' ')
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        i += 3;
    }

    *pp_out      = p_out;
    *p_i_out_len = i_out_len;

    return 0;
error:
    if (p_out)
        free(p_out);
    return -1;
}

static uint64_t GetSysTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t) (tv.tv_sec * 1000000 + tv.tv_usec);
}

void IMURequester::DecodeIMUData(const char *pData, size_t nSize, IMU_t *pIMU)
{
    system_clock::time_point timeNow;
    uint64_t                 time;

    if (pData == nullptr || nSize <= 0)
    {
        printf("%s:null\n", __func__);
        return;
    }

    if (nSize != sizeof(EvIMUReplyData_t))
    {
        printf("%s:size is not the same\n", __func__);
        return;
    }
    else
    {
        EvIMUReplyData_t *pEvIMUReplyData = (EvIMUReplyData_t *) pData;
        unsigned short   usDegrees        = ntohs(pEvIMUReplyData->usDegrees);
        float            fOriginalX       = FloatSwap(pEvIMUReplyData->fOriginalX);
        float            fOriginalY       = FloatSwap(pEvIMUReplyData->fOriginalY);
        unsigned short   righttof         = ntohs(pEvIMUReplyData->usRightToFDistance);
        unsigned short   lefttof          = ntohs(pEvIMUReplyData->usLeftToFDistance);
        unsigned char    motiondir        = pEvIMUReplyData->kMotionDirection;

        pIMU->id              = pEvIMUReplyData->ucID;
        pIMU->degree          = usDegrees;
        pIMU->x               = fOriginalX;
        pIMU->y               = fOriginalY;
        pIMU->rightSingleToF  = righttof;
        pIMU->leftSingleToF   = lefttof;
        pIMU->motionDirection = motiondir;

        if (m_PreIMUID == -1)
        {
            m_PreIMUID = pIMU->id;
            m_PreTime  = system_clock::now();
        }
        else
        {
            timeNow = system_clock::now();
            auto   duration = duration_cast<microseconds>(timeNow - m_PreTime);
            double ms       = (double(duration.count()) * microseconds::period::num / microseconds::period::den);
            time = GetSysTime();

            if (m_PreIMUID + 1 == pIMU->id || (m_PreIMUID == 255 && pIMU->id == 0))
            {
            }
            else
            {
                m_TotalLost++;
            }
//            FR_PRINT_GREEN("duration: %f sec, id: %d\n", ms, pIMU->id);
//            printf("%" PRIu64",%f,%f,%d,%f\n", time, fOriginalX, fOriginalY, usDegrees, ms);
//            fprintf(fp, "%" PRIu64",%f,%f,%d,%f\n", time, fOriginalX, fOriginalY, usDegrees, ms);

            m_TotalRecvCount++;

            if (ms * 1000 > IMU_TIMEDOUT3)
            {
                if (m_TotalS == 0)
                {
                    m_TotalS = ms;
                }
                else
                {
                    printf("m_TotalS: %f sec\n", m_TotalS);
                    m_TotalS = ms;
                }
            }
            else
            {
                m_TotalS += ms;
            }

            if (ms * 1000 < IMU_TIMEDOUT4)
            {
                m_TotalRecvShortCount1++;
                FR_PRINT_CYAN("duration: %f sec, id: %d\n", ms, pIMU->id);
            }

            if (ms * 1000 > IMU_TIMEDOUT3)
            {
                m_TotalRecvLongCount3++;
                FR_PRINT_BLUE("duration: %f sec, id: %d\n", ms, pIMU->id);
            }
            else if (ms * 1000 > IMU_TIMEDOUT2)
            {
                m_TotalRecvLongCount2++;
                FR_PRINT_BLUE("duration: %f sec, id: %d\n", ms, pIMU->id);
            }
            else if (ms * 1000 > IMU_TIMEDOUT1)
            {
                m_TotalRecvLongCount1++;
                FR_PRINT_BLUE("duration: %f sec, id: %d\n", ms, pIMU->id);
            }

//            printf(
//                "RecvTotal: %" PRIu64", timedout in %dms:%" PRIu64" (%.2f%%), timedout in %dms: %" PRIu64"(%.2f%%), timedout in %dms: %" PRIu64"(%.2f%%), time less %dms: %" PRIu64"(%.2f%%), lost:%" PRIu64"\r",
//                m_TotalRecvCount,
//                IMU_TIMEDOUT1, m_TotalRecvLongCount1, (double) m_TotalRecvLongCount1 / m_TotalRecvCount * 100,
//                IMU_TIMEDOUT2, m_TotalRecvLongCount2, (double) m_TotalRecvLongCount2 / m_TotalRecvCount * 100,
//                IMU_TIMEDOUT3, m_TotalRecvLongCount3, (double) m_TotalRecvLongCount3 / m_TotalRecvCount * 100,
//                IMU_TIMEDOUT4, m_TotalRecvShortCount1, (double) m_TotalRecvShortCount1 / m_TotalRecvCount * 100,
//                m_TotalLost);
            fflush(stdout);

            m_PreIMUID = pIMU->id;
            m_PreTime  = timeNow;
        }
    }
}

int IMURequester::Send(int fd, unsigned char id)
{
    char *pSendData   = nullptr;
    int  iSendDataLen = 0;
    char buff[18]     = "AA 00 02 81 00 81";
    int  ret;

    if (hexstring_to_bytearray(buff, &pSendData, &iSendDataLen))
    {
        perror("HexStringToBytearray failed\n");
        return -1;
    }
    pSendData[4] = id;
    pSendData[5] = pSendData[4] + pSendData[3];
    ret = write(fd, pSendData, iSendDataLen);

//    DebugUtil::PrintAsHexString("Send", (unsigned char *) pSendData, iSendDataLen);

    if (pSendData)
    {
        free(pSendData);
        pSendData = nullptr;
    }
    if (ret < 0)
        return -1;
    else
        return 0;
}

void IMURequester::RecvRoutine()
{
    int            iReadLen;
    fd_set         rd;
    char           buf[MAX_LEN];
    int            state    = 0;
    int            recv_len = 0;
    int            len_temp = 0;
    int            ret      = 0;
    struct timeval tv;

    printf("Start RecvThread...\n");
    pthread_barrier_wait(&m_ThreadIsRunning);

    //static system_clock::time_point old = system_clock::now();

    while (true)
    {
        FD_ZERO(&rd);
        FD_SET(m_nUsartfd, &rd);
        memset(buf, 0, sizeof(buf));

        tv.tv_sec  = 0;
        tv.tv_usec = 20000;

//        system_clock::time_point old = system_clock::now();

        ret = select(m_nUsartfd + 1, &rd, nullptr, nullptr, nullptr);

//        system_clock::time_point now = system_clock::now();
//        auto duration = duration_cast<microseconds>(now - old);
//        FR_PRINT_MAGENTA("\nTime duration between %s: %f sec.\n", "select", double(duration.count()) * microseconds::period::num / microseconds::period::den);
//        old = now;

        if (ret < 0)
        {
            perror("select error\n");
        }
        else if (ret == 0)
        {
            printf("timeout\n");
//            system_clock::time_point now = system_clock::now();
//            auto duration = duration_cast<microseconds>(now - old);
//            FR_PRINT_MAGENTA("Time duration between %s: %f sec.\n", "select", double(duration.count()) * microseconds::period::num / microseconds::period::den);
//            old = now;
            iReadLen = read(m_nUsartfd, buf, MAX_LEN);
            if (iReadLen > 0)
            {
                printf("timeout iReadLen:%d\n", iReadLen);
            }
        }
        else
        {
//            iReadLen = read(m_nUsartfd, buf, MAX_LEN);
//            if (iReadLen > 36)
//            {
////                    auto duration = duration_cast<microseconds>(now - old);
////                    FR_PRINT_MAGENTA("Time duration between %s: %f sec.\n", "read", double(duration.count()) * microseconds::period::num / microseconds::period::den);
//                FR_PRINT_YELLOW("iReadLen:%d\n", iReadLen);
//            }

            if (state == 0)
            {
                iReadLen = read(m_nUsartfd, buf, 1);
                if (iReadLen == 1 && (*(buf) & 0xFF) == PacketHeader)
                {
                    state = 1;
                    recv_len += iReadLen;
                }
                else
                {
                    printf("Recv wrong code:[0x%02X]\n", *buf);
                }
            }
            else if (state == 1)
            {
                iReadLen = read(m_nUsartfd, buf + recv_len, 2);
                if (iReadLen == 1)
                {
                    state    = 2;
                    len_temp = (*(buf + recv_len) << 8) & 0xFF00;
                    recv_len += iReadLen;
                }
                else if (iReadLen == 2)
                {
                    state    = 3;
                    len_temp = ((*(buf + recv_len) << 8) & 0xFF00) + (*(buf + recv_len + 1) & 0xFF) + 1;
                    recv_len += iReadLen;
                }
            }
            else if (state == 2)
            {
                iReadLen = read(m_nUsartfd, buf + recv_len, 1);
                if (iReadLen == 1)
                {
                    state = 3;
                    len_temp += (*(buf + recv_len) & 0xFF) + 1;
                    recv_len += iReadLen;
                }
            }
            else
            {
                static int last = 0;
                iReadLen = read(m_nUsartfd, buf + recv_len + last, len_temp - last);
                if (iReadLen < (len_temp - last) && iReadLen > 0)
                {
                    last += iReadLen;
                }
                else
                {
                    recv_len += len_temp;
                    last = 0;

                    state = 0;
                    if (recv_len >= 5)
                    {
                        char cmd = buf[3];
                        switch (cmd & 0xFF)
                        {
                            case 0x81:
                            {
                                IMU_t imu = {0};

//                                    DebugUtil::PrintAsHexString("Recv", (unsigned char *) buf, recv_len);

                                pthread_mutex_lock(&m_ReceivedMutex);
                                DecodeIMUData(buf + 4, recv_len - 5, &imu);
                                memcpy(&m_stIMUTempBuffer, &imu, sizeof(imu));
                                m_bReceivedIMUData = true;
                                pthread_cond_signal(&m_ReceivedCond);
                                pthread_mutex_unlock(&m_ReceivedMutex);
                                break;
                            }
                            default:;
                        }
                    }

                    recv_len = 0;
                }
            }
        }
    }
}

void IMURequester::RecvRoutine2()
{
    int  iReadLen;
    char buf[MAX_LEN];
    int  state    = 0;
    int  recv_len = 0;
    int  len_temp = 0;
    int  ret;

    struct pollfd fds[1];
    fds[0].fd     = m_nUsartfd;
    fds[0].events = POLLIN;

    static system_clock::time_point old = system_clock::now();

    printf("Start RecvThread...\n");
    printf("Use poll\n");
    pthread_barrier_wait(&m_ThreadIsRunning);

    while (true)
    {
        ret = poll(fds, 1, 20);

        system_clock::time_point                                              now      = system_clock::now();
        auto                                                                  duration = duration_cast<microseconds>(
            now - old);
        FR_PRINT_MAGENTA("Time duration between %s: %f sec.\n", "poll",
                         double                                               (duration.count()) * microseconds::period::num / microseconds::period::den);
        old = now;

        if (ret == -1)
        {
            perror("poll()");
        }
        else if (ret == 0)
        {
            printf("timeout\n");
            iReadLen = read(m_nUsartfd, buf, MAX_LEN);
            if (iReadLen > 0)
            {
                printf("timeout iReadLen:%d\n", iReadLen);
            }
        }
        else
        {
            if ((fds[0].revents & POLLIN) == POLLIN)
            {
                iReadLen = read(m_nUsartfd, buf, MAX_LEN);
                if (iReadLen > 36)
                {
                    FR_PRINT_YELLOW("iReadLen:%d\n", iReadLen);
                }
            }
        }
    }
}

void *IMURequester::RecvThread(void *arg)
{
    IMURequester *pIMURequester = (IMURequester *) arg;

    prctl(PR_SET_NAME, "IMURequester-RX");

    pIMURequester->RecvRoutine();

    pthread_exit(nullptr);
}

int IMURequester::GetIMU(IMU_t *pIMU, unsigned char index)
{
//    if (Send(m_nUsartfd, index) < 0)
//    {
//        printf("Send failed\n");
//        return -1;
//    }

//    if (m_PreIMUID == -1)
//    {
//
//    }

    m_TotalRequestCount++;
//    printf("SendTotal: %llu, timedout in %dms: %llu (%.2f%%)\r", m_TotalRequestCount, IMU_TIMEDOUT1,
//           m_TotalTimedoutCount, (double) m_TotalTimedoutCount / m_TotalRequestCount * 100);
//    fflush(stdout);

    pthread_mutex_lock(&m_ReceivedMutex);
    while (!m_bReceivedIMUData)
    {
        static struct timespec timeout;
        memset(&timeout, 0x00, sizeof(timeout));
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += IMU_TIMEDOUT1 * 1000000;
//        TICK(b);
        if (pthread_cond_timedwait(&m_ReceivedCond, &m_ReceivedMutex, &timeout) == ETIMEDOUT)
        {
            m_TotalTimedoutCount++;
//            TOCK(b, "get imu timedout");
//            printf("Timedout in %dms: %.2f%% (%llu/%llu)\n", IMU_TIMEDOUT1,
//                   (double) m_TotalTimedoutCount / m_TotalRequestCount * 100, m_TotalTimedoutCount,
//                   m_TotalRequestCount);
            pthread_mutex_unlock(&m_ReceivedMutex);
            return -1;
        }
    }

//    TOCK(a, "received imu");

    m_bReceivedIMUData = false;

//    if (index != m_stIMUTempBuffer.id)
//    {
//        printf("receive wrong imu id: %d, should be %d\n", m_stIMUTempBuffer.id, index);
//        pthread_mutex_unlock(&m_ReceivedMutex);
//        return -1;
//    }
    memcpy(pIMU, &m_stIMUTempBuffer, sizeof(IMU_t));
//    pIMU->id = index;
    pthread_mutex_unlock(&m_ReceivedMutex);

    return 0;
}
