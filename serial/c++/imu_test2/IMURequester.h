#ifndef _IMUREQUESTER_H
#define _IMUREQUESTER_H

#include <stdio.h>
#include <cstdint>
#include <pthread.h>
#include <chrono>

#define IMU_TIMEDOUT1 50
#define IMU_TIMEDOUT2 100
#define IMU_TIMEDOUT3 400
#define IMU_TIMEDOUT4 10

struct IMU_t
{
    uint8_t  id;
    uint8_t  motionDirection;
    float    x;
    float    y;
    uint16_t degree;
    int16_t  rightSingleToF;
    int16_t  leftSingleToF;
} __attribute__((__packed__));

using namespace std;
using namespace chrono;

class IMURequester
{
public:
    IMURequester();

    ~IMURequester();

    int GetIMU(IMU_t *pIMU, unsigned char index);

private:
    pthread_t         m_RxID;
    pthread_barrier_t m_ThreadIsRunning;
    int               m_nUsartfd;
    bool              m_bReceivedIMUData;
    pthread_mutex_t   m_ReceivedMutex;
    pthread_cond_t    m_ReceivedCond;
    IMU_t             m_stIMUTempBuffer;
    uint64_t          m_TotalRequestCount;
    uint64_t          m_TotalTimedoutCount;

    int16_t                  m_PreIMUID             = -1;
    system_clock::time_point m_PreTime;
    uint64_t                 m_TotalRecvCount       = 0;
    uint64_t                 m_TotalRecvLongCount1  = 0;
    uint64_t                 m_TotalRecvLongCount2  = 0;
    uint64_t                 m_TotalRecvLongCount3  = 0;
    uint64_t                 m_TotalRecvShortCount1 = 0;
    uint64_t                 m_TotalLost            = 0;

    double m_TotalS = 0;

    FILE *fp;

    static int hexstring_to_bytearray(char *p_hexstring, char **pp_out, int *p_i_out_len);

    void DecodeIMUData(const char *pData, size_t nSize, IMU_t *pIMU);

    static int Send(int fd, unsigned char id);

    void RecvRoutine();

    void RecvRoutine2();

    static void *RecvThread(void *arg);
};

#endif
