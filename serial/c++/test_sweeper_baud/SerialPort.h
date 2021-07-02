/*
 * SerialPort.h
 *
 *  Created on: 2019,11,01
 *      Author: wangxiaojia5
 */

#pragma once

#include <termios.h>
#include <sys/types.h>

#include <string>
#include <mutex>
#include <thread>
#include <iostream>
#include "McuProtocol.h"
#include "barrier.h"
#include "../../../common/Locker.h"

using namespace std;

static const int COMM_BUFFER_MAX_LEN = 2 * 1024;

static const int COMM_MAX_SEND_LEN = 415;
static const int COMM_MIN_RECV_LEN = sizeof(UartFrame);

enum UART_RECV_STATE
{
    WAIT_MAGIC_NUMBER,
    WAIT_MAGIC_NUMBER_SECOND_BYTE,
    WAIT_LENGTH,
    WAIT_LENGTH_SECOND_BYTE,
    WAIT_REST_FRAME_AND_DATA,
};

typedef void(*CommRecvCallBack)(UartFrame *);

class SerialPort
{
public:
    SerialPort(std::string name, int baud);

    ~SerialPort() = default;

    bool Init(CommRecvCallBack recvCallBack, bool bBlock);

    bool DeInit();

private:
    int         m_Usartfd;
    std::string m_TtyName;
    int         m_nSpeed;

    unsigned char gRecv_Buff[COMM_BUFFER_MAX_LEN];
    int           gRecv_Count;

    bool m_bRunning;

private:
    swp::barrier     m_InitBarrier;
    std::thread      m_RecvThrd;
    std::thread      m_SendThrd;
    CommRecvCallBack m_RecvCallBack;

    mutex m_mutex;

    int CheckRecvData(UartFrame *pFrame);

    int HandleRecvData(UartFrame *pFrame);

    void RecvingThread();
};
