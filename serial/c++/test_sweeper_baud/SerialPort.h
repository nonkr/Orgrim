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
#include "../../../common/Locker.h"

static const int COMM_HEADER			= 0xAA;
static const int COMM_INTERVAL			= 0xFF;
static const int COMM_BUFFER_MAX_LEN	= 1680;

static const int COMM_DATA_MAX_LEN		= 600;
static const int COMM_MIN_SEND_LEN		= 4;
static const int COMM_MAX_SEND_LEN		= 415;
static const int COMM_MIN_RECV_LEN		= 4;
static const int COMM_CHECK_SUM_LEN		= 1;

#pragma pack(push,1)
struct SerialPortHeader
{
	uint8_t head = COMM_HEADER;
	uint16_t len = 0;
	uint8_t type = 0;
};

struct SerialPortData
{	
	uint16_t len = 0; //not in real body
	uint8_t type = 0; //not in real body
	uint8_t *data = nullptr;
};

struct SerialPortEnd
{
	uint8_t checkSum = 0;
};

#pragma pack(pop)

typedef void(*CommRecvCallBack)(SerialPortData*);

class SerialPort
{
public:
	SerialPort(std::string name, int baud);
    virtual ~SerialPort();
    bool Init(CommRecvCallBack recvCallBack, bool bBlock);
	int AddToSendBuf(SerialPortData * pData);
	void ResetCounts();
	inline int GetSendCount() { return m_SendCnt; }
	inline int GetRecvCount() { return m_RecvCnt; }
private:
    int         m_Usartfd;
    std::string m_TtyName;
    int         m_nSpeed;
    int         m_nDatabits;
    int         m_nStopbits;
    char        m_nParity;

    unsigned char gSend_Buff[COMM_BUFFER_MAX_LEN];
    unsigned char gRecv_Buff[COMM_BUFFER_MAX_LEN];
    int           gSend_Count;
    int           gRecv_Count;

    bool m_bRunning;

	int m_SendCnt = 0;
	int m_RecvCnt = 0;

private:
    barrier     m_InitBarrier;
    std::thread m_RecvThrd;
    std::thread m_SendThrd;
	CommRecvCallBack m_RecvCallBack;

    bool m_bSendData = false;
    cond m_CondSendData;

	int SetBaud();
	int SetOpt();
	int PackSendData(SerialPortData * pData, unsigned char * pPackedData);

    int CheckRecvData(unsigned char *buf, size_t recv_len);
    int HandleRecvData(unsigned char *buf, int len);

    void SendingThread();
    void RecvingThread();

};
