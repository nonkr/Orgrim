#ifndef _IMUREQUESTER_H
#define _IMUREQUESTER_H

#include <stdio.h>
#include <cstdint>
#include <pthread.h>

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

#define IMU_TIMEDOUT1 50

class IMURequester
{
public:
    IMURequester();

    ~IMURequester();

    int GetIMU(IMU_t *pIMU, unsigned char index);

    void Stop();

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

    static int hexstring_to_bytearray(char *p_hexstring, char **pp_out, int *p_i_out_len);

    void DecodeIMUData(const char *pData, size_t nSize, IMU_t *pIMU);

    static int Send(int fd, unsigned char id);

    void RecvRoutine();

    static void *RecvThread(void *arg);
};

#endif
