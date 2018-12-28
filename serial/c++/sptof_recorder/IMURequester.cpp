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
//    short             sLeftSpeed;                      // speed of left code disc
//    short             sRightSpeed;                     // speed of right code disc
    unsigned short    usBatteryVoltage;             // battery voltage
}  __attribute__((__packed__));

IMURequester::IMURequester() : m_ThreadIsRunning(2)
{
    m_bReceivedIMUData   = false;
    m_nUsartfd           = -1;

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

    if (pthread_create(&m_RxID, nullptr, RecvThread, this))
    {
        printf("pthread_create RecvThread failed\n");
        return;
    }

    m_ThreadIsRunning.wait();
}

IMURequester::~IMURequester()
{
    pthread_join(m_RxID, nullptr);
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

void IMURequester::DecodeIMUData(const char *pData, size_t nSize, IMU_t *pIMU)
{
    if (pData == nullptr || nSize <= 0)
    {
        printf("%s:nullptr\n", __func__);
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
    }
}

void IMURequester::RecvRoutine()
{
    int    iReadLen;
    fd_set rd;
    char   buf[MAX_LEN];
    int    state    = 0;
    int    recv_len = 0;
    int    len_temp = 0;

    m_ThreadIsRunning.wait();

    while (true)
    {
        FD_ZERO(&rd);
        FD_SET(m_nUsartfd, &rd);
        memset(buf, 0, sizeof(buf));
        while (FD_ISSET(m_nUsartfd, &rd))
        {
            if (select(m_nUsartfd + 1, &rd, nullptr, nullptr, nullptr) < 0)
            {
                perror("select error\n");
            }
            else
            {
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
                            switch (cmd)
                            {
                                case 0x81:
                                {
                                    IMU_t imu = {0};

//                                    DebugUtil::PrintAsHexString("Recv", (unsigned char *) buf, recv_len);

                                    m_ReceivedCond.lock();
                                    DecodeIMUData(buf + 4, recv_len - 5, &imu);
                                    memcpy(&m_stIMUTempBuffer, &imu, sizeof(imu));
                                    m_bReceivedIMUData = true;
                                    m_ReceivedCond.signal();
                                    m_ReceivedCond.unlock();
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
}

void *IMURequester::RecvThread(void *arg)
{
    IMURequester *pIMURequester = (IMURequester *) arg;

    prctl(PR_SET_NAME, "IMURequester-RX");

    pIMURequester->RecvRoutine();

    pthread_exit(nullptr);
}

int IMURequester::GetIMU(IMU_t *pIMU)
{
    m_ReceivedCond.lock();
    while (!m_bReceivedIMUData)
    {
        m_ReceivedCond.wait();
    }

    m_bReceivedIMUData = false;

    memcpy(pIMU, &m_stIMUTempBuffer, sizeof(IMU_t));
    m_ReceivedCond.unlock();

    return 0;
}

void IMURequester::Stop()
{
    pthread_cancel(m_RxID);
}
