/*
* SerialPort.cpp
*
*  Created on: 2019,11,01
*      Author: wangxiaojia5
*/


#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <condition_variable>
#include <sys/ioctl.h>

#include "SerialPort.h"
#include "../../../common/PrintUtil.h"
#include "../../../common/DebugUtil.h"

using namespace std;
using namespace chrono;

int speed_arr2[] = {B1500000, B460800, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400,
                    B19200,
                    B9600,
                    B4800, B2400, B1200, B300,};
int name_arr2[]  = {1500000, 460800, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800,
                    2400,
                    1200, 300,};

SerialPort::SerialPort(std::string name, int baud) : m_InitBarrier(3)
{
    m_Usartfd   = -1;
    m_TtyName   = name;
    m_nSpeed    = baud;
    m_nDatabits = 8;
    m_nStopbits = 1;
    m_nParity   = 'n';
    m_bRunning  = false;
}

SerialPort::~SerialPort()
{
    m_bRunning = false;

    ioctl(m_Usartfd, TIOCSTI, "please unblock");

    if (m_RecvThrd.joinable())
    {
        m_RecvThrd.join();
    }

    if (m_SendThrd.joinable())
    {
        m_CondSendData.lock();
        m_bSendData = true;
        m_CondSendData.signal();
        m_CondSendData.unlock();
        m_SendThrd.join();
    }

    close(m_Usartfd);

}

bool SerialPort::Init(CommRecvCallBack recvCallBack, bool bBlock)
{
    m_RecvCallBack = recvCallBack;

    if (bBlock)
    {
        m_Usartfd = open(m_TtyName.c_str(), O_RDWR);
    }
    else
    {
        m_Usartfd = open(m_TtyName.c_str(), O_RDWR | O_NONBLOCK);
    }

    if (m_Usartfd <= 0)
    {
//        LOGD("%s Open Failed !!!!", m_TtyName.c_str());
        return false;
    }

    if (SetBaud() < 0)
    {
//        LOGD("Failed to Set Baud !!!");
        return false;
    }

    if (SetOpt() < 0)
    {
//        LOGD("Failed to Set Opt !!!");
        return false;
    }

    gSend_Count = 0;
    memset(gSend_Buff, 0, sizeof(gSend_Buff));
    gRecv_Count = 0;
    memset(gRecv_Buff, 0, sizeof(gRecv_Buff));

    if (!m_bRunning)
    {
        m_bRunning = true;

        m_SendThrd = std::thread(&SerialPort::SendingThread, this);
        m_RecvThrd = std::thread(&SerialPort::RecvingThread, this);

        m_InitBarrier.wait();
    }

//    LOGD("Serial port %s init success !!!", m_TtyName.c_str());
    return true;
}


int SerialPort::SetBaud()
{
    unsigned char  i;
    int            status;
    struct termios Opt;
    tcgetattr(m_Usartfd, &Opt);
    for (i = 0; i < (sizeof(speed_arr2) / sizeof(speed_arr2[0])); i++)
    {
        if (m_nSpeed == name_arr2[i])
        {
            tcflush(m_Usartfd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr2[i]);
            cfsetospeed(&Opt, speed_arr2[i]);
            status = tcsetattr(m_Usartfd, TCSANOW, &Opt);
            if (status != 0)
            {
                perror("tcsetattr fd1");
                return -1;
            }
            tcflush(m_Usartfd, TCIOFLUSH);
        }
    }
    return 0;
}

int SerialPort::SetOpt()
{
    struct termios options;
    if (tcgetattr(m_Usartfd, &options) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (m_nDatabits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            FR_PRINT_RED("UART", "Unsupported data size\n");
            return -1;
    }
    switch (m_nParity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;    /* Clear parity enable */
            options.c_iflag &= ~INPCK;    /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;    /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;    /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;    /* Disnable parity checking */
            break;
        case 'S':
        case 's':            /*as no parity */
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            FR_PRINT_RED("UART", "Unsupported parity\n");
            return -1;
    }

    switch (m_nStopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            FR_PRINT_RED("UART", "Unsupported stop bits\n");
            return -1;
    }

    options.c_lflag &= ~(ICANON | ISIG);
    options.c_iflag &= ~(ICRNL | IGNCR);

    // reference: https://linux.die.net/man/3/cfmakeraw
    cfmakeraw(&options);

    /* Set input parity option */
    if (m_nParity != 'n')
        options.c_iflag |= INPCK;
    tcflush(m_Usartfd, TCIFLUSH);
    options.c_cc[VTIME] = 150;
    options.c_cc[VMIN]  = 0;    /* Update the options and do it NOW */
    if (tcsetattr(m_Usartfd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}

int SerialPort::PackSendData(SerialPortData *pData, unsigned char *pPackedData)
{
    if ((pData->len > 1) && (nullptr == pData->data))
        return -1;

    if (nullptr == pPackedData)
        return -1;

    SerialPortHeader *header = (SerialPortHeader *) pPackedData;
    header->head = COMM_HEADER;
    header->len  = pData->len + 1; //1 for type len
    header->type = pData->type;

    uint8_t *data      = pPackedData + sizeof(SerialPortHeader);
    uint8_t ucChecksum = pData->type;
    if (pData->len > 1)
    {
        for (int i = 0; i < pData->len; i++)
        {
            data[i] = pData->data[i];
            ucChecksum += data[i];
        }
    }

    data[pData->len] = ucChecksum;

    return 0;
}

int SerialPort::AddToSendBuf(SerialPortData *pData)
{
    int ret = -1;
    if (nullptr == pData)
        return ret;

    unsigned char packedData[COMM_MAX_SEND_LEN] = {0x00};

    int length = sizeof(SerialPortHeader) + pData->len + sizeof(SerialPortEnd);

    if (PackSendData(pData, packedData) < 0)
    {
//        LOGD("Pack Data Failed!!!");
        return ret;
    }

    if ((gSend_Count + length) < COMM_BUFFER_MAX_LEN)
    {
        m_CondSendData.lock();
        memcpy(gSend_Buff + gSend_Count, packedData, length);
        gSend_Count += length;
        ret         = 0;
        m_bSendData = true;
        m_CondSendData.signal();
        m_SendCnt++;
        m_CondSendData.unlock();
    }
    else
    {
        ret = -2;
        //LOGD("Send Buffer Fulled !!!!!!");
    }

    return ret;
}

void SerialPort::ResetCounts()
{
    m_SendCnt = 0;
    m_RecvCnt = 0;
}

/**	@fn	     CheckRecvData(unsigned char* buf,unsigned char recv_len)
 *	@brief	 check the CRCbyte
 *	@param   buf--------recv data buf 
 *  @param   recv_len --- recv data length
 *	@return	 correct data:0,  wrong data: -1  
 *  @note    
*/
int SerialPort::CheckRecvData(unsigned char *buf, size_t recv_len)
{
    if (nullptr == buf)
        return -1;

    SerialPortHeader *header = (SerialPortHeader *) buf;
    uint8_t          head    = header->head;
    uint16_t         len     = header->len - 1;//1 for type len
    uint8_t          type    = header->type;

    //check head
    if (head != COMM_HEADER)
    {
//        LOGD("Cmd Head wrong !!!");
        return -1;
    }

    //check data len
    if (recv_len < len + sizeof(SerialPortHeader))
    {
//        LOGD("Cmd not completion!\n");
        return -1;
    }

    //check sum
    uint8_t  checkSum = type;
    uint8_t  *data    = buf + sizeof(SerialPortHeader);
    for (int i        = 0; i < len; i++)
    {
        checkSum += data[i];
    }

    if (checkSum != data[len])
    {
        FR_PRINT_RED("UART", "uart checksum wrong\n");
        DebugUtil::PrintAsHexString("RecvFailed", buf, recv_len, false);
        return -1;
    }

    return 0;
}


/**	@fn	    HandleRecvData(unsigned char * buf,int len)
 *	@brief	 handle received data
 *	@param   buf  ---  the data need to process
 *	@param   len  ---  data length
 *	@return	  0:  suceess
 *			  -1:  data is null or not commplete
 			  -2:   data check wrong
 			  -3:   other reason
 *  @note    
*/
int SerialPort::HandleRecvData(unsigned char *buf, int len)
{
    if ((nullptr == buf) || (len < COMM_MIN_RECV_LEN))
    {
//        UART_PRINT("recv data is wrong or not integrity!\n");
        return -1;
    }

    if (CheckRecvData(buf, len) < 0)
    {
        FR_PRINT_RED("UART", "Check received cmd data failure\n");
        return -2;
    }

    SerialPortHeader *header = (SerialPortHeader *) buf;
    SerialPortData   body;
    body.type = header->type;
    body.len  = header->len - 1; //l for type len
    body.data = buf + sizeof(SerialPortHeader);

    //receive call back
    m_RecvCallBack(&body);
    m_RecvCnt++;
    return 0;
}

/**	@fn	     SendingThread()
 *	@brief	 the thread of send data 
 *	@param   nullptr
 *	@return	 nullptr
 *  @note    
*/
void SerialPort::SendingThread()
{
    prctl(PR_SET_NAME, "UART_T");

//    MsgMgr           &pMQ          = MsgMgr::getInstance();
//    HIK_ABNORMALType kAbnormalType = AB_USART_FAULT;

    m_InitBarrier.wait();
    while (m_bRunning)
    {
        unsigned char            cSendBuff[COMM_BUFFER_MAX_LEN];
        int                      len = 0;
        system_clock::time_point tpTmp;

        m_CondSendData.lock();
        while (!m_bSendData)
        {
            m_CondSendData.wait();
        }

        if (!m_bRunning)
        {
            m_CondSendData.unlock();
            break;
        }

        if (gSend_Count >= COMM_BUFFER_MAX_LEN)
        {
            gSend_Count = 0;
        }
        if (gSend_Count > 0)
        {
            len = write(m_Usartfd, gSend_Buff, gSend_Count); //none block !!!!

            if (len < 0)
            {
                FR_PRINT_RED("UART", "send data to PORT failed!\n");
//                SYSLOG_ERROR("%s", "send data to PORT failed!");
//                pMQ.Publish(T_HIK_FAULT, (unsigned char *) &kAbnormalType, 1);
                m_CondSendData.unlock();
                continue;
            }
            else if (0 == len)
            {
                m_CondSendData.unlock();
                continue;
            }
            else
            {
//                if (g_stRobotOption.stAppDebugSwitch.iUARTPrintSend)
//                {
//                    DebugUtil::PrintAsHexString("Send", gSend_Buff, len, false);
//                }

                if (len == gSend_Count)
                    gSend_Count = 0;
                else if (len < gSend_Count)
                {
                    gSend_Count -= len;
                    memset(cSendBuff, 0, gSend_Count);
                    memcpy(cSendBuff, gSend_Buff + len, gSend_Count);
                    memcpy(gSend_Buff, cSendBuff, gSend_Count);
                }
            }
        }

        m_bSendData = false;

        m_CondSendData.unlock();
    }
}

/**	@fn	     RecvingThread()
 *	@brief	 the thread of receive data
 *	@param   nullptr
 *	@return	 nullptr
 *  @note    
*/
void SerialPort::RecvingThread()
{
    int           iReadLen;
    int           state    = 0;
    int           len_temp = 0;
    int           recv_len = 0;
    unsigned char buf[COMM_BUFFER_MAX_LEN];
    memset(buf, 0, sizeof(buf));
    fd_set rd;

    prctl(PR_SET_NAME, "UART_R");

    m_InitBarrier.wait();

    while (m_bRunning)
    {
        FD_ZERO(&rd);
        FD_SET(m_Usartfd, &rd);
        while (FD_ISSET(m_Usartfd, &rd))
        {
            if (select(m_Usartfd + 1, &rd, nullptr, nullptr, nullptr) < 0)
            {
                perror("select error\n");
            }
            else
            {
                if (!m_bRunning) break;

                /*use status machine to control one commplete frame of data received*/
                if (state == 0) // handle first byte        
                {
                    iReadLen = read(m_Usartfd, buf, 1);

                    if (iReadLen == 1 && *(buf) == COMM_HEADER)
                    {
                        //FR_PRINT_RED("UART", "\n Recv header : [%02X]\n", *(buf));
                        state = 1;
                        recv_len += iReadLen;
                    }
                    else
                    {
                        FR_PRINT_RED("UART", "Recv: [%02X]\n", *(buf));
                    }
                }
                else if (state == 1) // receive the first byte of 2bytes data length 
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 2);
                    if (iReadLen == 1)
                    {
                        state    = 2;
                        len_temp = *(buf + recv_len);
                        recv_len += iReadLen;
                        // at present, the max value of length is PROTO_LENGTH_MAX_NUMBER, so the first byte should be 0
                        if (len_temp > 0)
                        {
                            recv_len = 0;
                            state    = 0;
                            FR_PRINT_RED("UART", "Error: length:[%d] > %d\n", len_temp, COMM_DATA_MAX_LEN);
                        }
                    }
                    else if (iReadLen == 2)
                    {
                        state    = 3;
                        len_temp = ((*(buf + recv_len + 1) << 8) & 0xFF00) + (*(buf + recv_len) & 0xFF) + 1;
                        recv_len += iReadLen;
                        // at present, the max value of length is PROTO_LENGTH_MAX_NUMBER
                        if (len_temp > COMM_DATA_MAX_LEN)
                        {
                            recv_len = 0;
                            state    = 0;
                            FR_PRINT_RED("UART", "Error: length:[%d] > %d\n", len_temp, COMM_DATA_MAX_LEN);
                        }
                    }
                }
                else if (state == 2) // receive the next byte of data length
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state = 3;
                        len_temp += (((*(buf + recv_len)) << 8) & 0xFF00) + 1;
                        recv_len += iReadLen;
                        // at present, the max value of length is PROTO_LENGTH_MAX_NUMBER
                        if (len_temp > COMM_DATA_MAX_LEN)
                        {
                            recv_len = 0;
                            state    = 0;
                            FR_PRINT_RED("UART", "Error: length:[%d] > %d\n", len_temp, COMM_DATA_MAX_LEN);
                        }
                    }
                }
                else // receive the rest data
                {
                    static int last = 0;
                    iReadLen = read(m_Usartfd, buf + recv_len + last, len_temp - last);
                    if (iReadLen < (len_temp - last) && iReadLen > 0)
                    {
                        last += iReadLen;
                    }
                    else
                    {
                        recv_len += len_temp;
                        last  = 0;
                        state = 0;

//                        if (g_stRobotOption.stAppDebugSwitch.iUARTPrintReceive)
//                        {
//                            DebugUtil::PrintAsHexString("Recv", buf, recv_len, true);
//                        }

                        if (HandleRecvData(buf, recv_len) < 0)
                        {
//                            FR_PRINT_RED("UART", "recv data parsed failed!\n");
                        }
                        recv_len = 0;
                    }
                }
            }
        }
    }
}
