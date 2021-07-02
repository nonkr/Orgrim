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
#include "serial.h"
#include "SysUtil.h"
#include "../../../common/PrintUtil.h"
#include "../../../common/TimeUtil.h"

using namespace std;
using namespace chrono;

static void PrintAsHexString(const int type, const unsigned char *pData, size_t len)
{
    char arrBuff[4096 * 2] = {0};
    int  iBuffOffset       = 0;
    int  iStrLen           = 0;

    char *pLabel = nullptr;
    char *color  = nullptr;
    if (type == 0)
    {
        pLabel = (char *) "Send";
        color  = (char *) KMAGENTA;
    }
    else if (type == 1)
    {
        pLabel = (char *) "Recv";
        color  = (char *) KCYAN;
    }
    else if (type == 2)
    {
        pLabel = (char *) "SendErr";
        color  = (char *) KRED;
    }
    else if (type == 3)
    {
        pLabel = (char *) "RecvErr";
        color  = (char *) KRED;
    }
    iStrLen      = strlen(pLabel) + 3 + 29 + strlen(color);
    snprintf(arrBuff + iBuffOffset, iStrLen, "[%s] %s%s:[", TimeUtil::TimeNowStringMS().c_str(), color, pLabel);
    iBuffOffset += iStrLen - 1;

    for (uint16_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            snprintf(arrBuff + iBuffOffset, 3, "%02X", pData[i]);
            iBuffOffset += 2;
        }
        else
        {
            snprintf(arrBuff + iBuffOffset, 4, "%02X ", pData[i]);
            iBuffOffset += 3;
        }
    }

    iStrLen = strlen("]\n") + 1 + strlen(KRESET);
    snprintf(arrBuff + iBuffOffset, iStrLen, "]%s\n", KRESET);
    iBuffOffset += iStrLen - 1;

    if (iBuffOffset > 0) FR_PRINT("DebugUtil", "%s", arrBuff);
}

static void PrintUartFrame(const char *pLabel, UartFrame *pFrame, bool bIgnore)
{
//    if (bIgnore)
//    {
//        if (g_stRobotOption.stAppDebugSwitch.iUARTPrintRecvIgnoreFastReg && pFrame->command == MCT_M2H_FAST_REG)
//            return;
//        if (g_stRobotOption.stAppDebugSwitch.iUARTPrintRecvIgnoreSlowReg && pFrame->command == MCT_M2H_SLOW_REG)
//            return;
//    }
#if 1
    uint16_t cChecksum     = 0;
    char     arrBuff[4096] = {0};
    int      iBuffOffset   = 0;
    int      iStrLen       = 0;

    iStrLen = strlen(pLabel) + 3;
    snprintf(arrBuff + iBuffOffset, iStrLen, "%s:[", pLabel);
    iBuffOffset += iStrLen - 1;

    // magic
    if (pFrame->magic == UART_FRAME_MAGIC_NUMBER)
    {
        iStrLen = strlen(KYELLOW) + 5 + strlen(KRESET) + 1;
        snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X %02X%s", KYELLOW, ((char *) pFrame)[0],
                 ((char *) pFrame)[1],
                 KRESET);
    }
    else
    {
        iStrLen = strlen(KRED) + 5 + strlen(KRESET) + 1;
        snprintf(arrBuff + iBuffOffset, iStrLen, "%s%02X %02X%s", KRED, ((char *) pFrame)[0], ((char *) pFrame)[1],
                 KRESET);
    }
    iBuffOffset += iStrLen - 1;

    // length
    iStrLen = strlen(KGREEN) + 6 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KGREEN, ((char *) pFrame)[2], ((char *) pFrame)[3],
             KRESET);
    iBuffOffset += iStrLen - 1;

    // id
    iStrLen = 12 + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %02X %02X %02X %02X", ((char *) pFrame)[4], ((char *) pFrame)[5],
             ((char *) pFrame)[6], ((char *) pFrame)[7]);
    iBuffOffset += iStrLen - 1;

    // command
    iStrLen = strlen(KLIMEGREEN) + 3 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X%s", KLIMEGREEN, ((char *) pFrame)[8], KRESET);
    iBuffOffset += iStrLen - 1;

    // options
    iStrLen = strlen(KORANGE) + 3 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X%s", KORANGE, ((char *) pFrame)[9], KRESET);
    iBuffOffset += iStrLen - 1;

    // sn
    iStrLen = strlen(KCYAN) + 6 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KCYAN, ((char *) pFrame)[10], ((char *) pFrame)[11],
             KRESET);
    iBuffOffset += iStrLen - 1;

    // ack
    iStrLen = strlen(KMAGENTA) + 6 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KMAGENTA, ((char *) pFrame)[12],
             ((char *) pFrame)[13], KRESET);
    iBuffOffset += iStrLen - 1;

    // sack
    iStrLen = strlen(KBLUE) + 6 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KBLUE, ((char *) pFrame)[14], ((char *) pFrame)[15],
             KRESET);
    iBuffOffset += iStrLen - 1;

    // frag_sn
    iStrLen = strlen(KCYAN) + 6 + strlen(KRESET) + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KCYAN, ((char *) pFrame)[16], ((char *) pFrame)[17],
             KRESET);
    iBuffOffset += iStrLen - 1;

    // checksum
    uint16_t checksumInData = pFrame->checksum;
    uint32_t id             = pFrame->id;
    pFrame->checksum = 0;
    pFrame->id       = 0;
    uint16_t checksum = SysUtil::CalcChecksum((void *) pFrame, pFrame->length + UART_FRAME_SIZE_BEFORE_WITH_LENGTH);
    pFrame->checksum = checksumInData;
    pFrame->id       = id;
    if (checksum == pFrame->checksum)
    {
        iStrLen = strlen(KPINK) + 6 + strlen(KRESET) + 1;
        snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KPINK, ((char *) pFrame)[18],
                 ((char *) pFrame)[19], KRESET);
    }
    else
    {
        iStrLen = strlen(KRED) + 6 + strlen(KRESET) + 1;
        snprintf(arrBuff + iBuffOffset, iStrLen, " %s%02X %02X%s", KRED, ((char *) pFrame)[20],
                 ((char *) pFrame)[21], KRESET);
    }
    iBuffOffset += iStrLen - 1;

    uint16_t dataLen = pFrame->length - UART_FRAME_SIZE_AFTER_LENGTH;
    uint8_t  *pData  = pFrame->data;

    for (uint16_t i = 0; i < dataLen; i++)
    {
        snprintf(arrBuff + iBuffOffset, 4, " %02X", pData[i]);
        iBuffOffset += 3;
    }

    iStrLen = strlen("]\n") + 1;
    snprintf(arrBuff + iBuffOffset, iStrLen, "]\n");
    iBuffOffset += iStrLen - 1;

    if (iBuffOffset > 0) FR_PRINT("DebugUtil", "%s", arrBuff);
#else
    //    FR_PRINT("DebugUtil", "%s", "Recv:[AA AA 54 01 5B 00 00 00 F3 00 00 00 00 00 00 00 00 00 B1 48 11 01 14 00 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01]\n");
    //    printf("%s", "Recv:[AA AA 54 01 5B 00 00 00 F3 00 00 00 00 00 00 00 00 00 B1 48 11 01 14 00 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01]\n");
        printf("%s", "Recv:[AA AA 54 01 5B 00 00 00 F3 00 00 00 00 00 00]\n");
#endif
}

SerialPort::SerialPort(std::string name, int baud) : m_InitBarrier(2)
{
    m_Usartfd  = -1;
    m_TtyName  = name;
    m_nSpeed   = baud;
    m_bRunning = false;
}

bool SerialPort::Init(CommRecvCallBack recvCallBack, bool bBlock)
{
    FR_PRINT_BLUE("uart", "Serial port init\n");
    m_RecvCallBack = recvCallBack;

    m_Usartfd = OpenTTY(m_TtyName.c_str(), bBlock);

    if (m_Usartfd <= 0)
    {
        FR_PRINT_RED("uart", "Failed to open %s\n", m_TtyName.c_str());
        return false;
    }

    if (SetBaud(m_Usartfd, m_nSpeed) < 0)
    {
        FR_PRINT_RED("uart", "Failed to set baud %d\n", m_nSpeed);
        return false;
    }

    if (SetOpt(m_Usartfd, 8, 1, 'n') < 0)
    {
        FR_PRINT_RED("uart", "Failed to set options\n");
        return false;
    }

    gRecv_Count = 0;
    memset(gRecv_Buff, 0, sizeof(gRecv_Buff));

    if (!m_bRunning)
    {
        m_bRunning = true;

        m_RecvThrd = std::thread(&SerialPort::RecvingThread, this);

        m_InitBarrier.wait();
    }

    FR_PRINT_BLUE("uart", "Serial port init done\n");
    return true;
}

bool SerialPort::DeInit()
{
    FR_PRINT_GREEN("uart", "SerialPort DeInit\n");

    m_bRunning = false;

    ioctl(m_Usartfd, TIOCSTI, "please unblock");

    if (m_RecvThrd.joinable())
    {
        m_RecvThrd.join();
    }

    close(m_Usartfd);

    FR_PRINT_GREEN("uart", "SerialPort DeInit done\n");
    return true;
}

/**	@fn	     CheckRecvData(unsigned char* buf,unsigned char recv_len)
 *	@brief	 check the frame header
 *	@param   pFrame ---  recv frame buffer
 *	@return	 correct data:0,  wrong data: -1
 *  @note
*/
int SerialPort::CheckRecvData(UartFrame *pFrame)
{
    // 校验magic number
    if (pFrame->magic != UART_FRAME_MAGIC_NUMBER)
    {
        FR_PRINT_RED("uart", "Wrong magic number:0x%04X\n", pFrame->magic);
        return -1;
    }

    // 校验checksum
    uint16_t checksumInData = pFrame->checksum;
    uint32_t id             = pFrame->id;
    pFrame->id       = 0;
    pFrame->checksum = 0;
    uint16_t checksum = SysUtil::CalcChecksum((void *) pFrame, pFrame->length + UART_FRAME_SIZE_BEFORE_WITH_LENGTH);
    pFrame->checksum = checksumInData;
    pFrame->id       = id;
    if (checksum != pFrame->checksum)
    {
        FR_PRINT_RED("UART", "uart checksum wrong\n");
        PrintAsHexString(3, (unsigned char *) pFrame, pFrame->length + UART_FRAME_SIZE_BEFORE_WITH_LENGTH);
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
#define FR_PRINT_G(tag, ...) printf(KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_G2(tag, ...)  printf(KGREEN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))

int SerialPort::HandleRecvData(UartFrame *pFrame)
{
    if (CheckRecvData(pFrame) < 0)
    {
//        FR_PRINT_RED("UART", "Check received cmd data failure\n");
        return -2;
    }
    //receive call back
//    FR_PRINT_G2("carrier", "Lidar count:%d\n", 10);
//    FR_PRINT_G2("carrier", "Lidar count:%d\n", 20);
//    FR_PRINT_G2("carrier", "Lidar count:%d\n", 30);
//    FR_PRINT_G2("carrier", "Lidar count:%d\n", 40);

//    test();

//    printf(KGREEN "Lidar count:%d\n" KRESET, 10);
//    printf(KGREEN "Lidar count:%d\n" KRESET, 20);
//    printf(KGREEN "Lidar count:%d\n" KRESET, 30);
//    printf(KGREEN "Lidar count:%d\n" KRESET, 40);

//    printf("id:%u\n", pFrame->id);
//    usleep(10000);
    m_RecvCallBack(pFrame);
    return 0;
}


/**	@fn	     RecvingThread()
 *	@brief	 the thread of receive data
 *	@param   nullptr
 *	@return	 nullptr
 *  @note
*/
void SerialPort::RecvingThread()
{
    int             iReadLen = 0;
    UART_RECV_STATE state    = WAIT_MAGIC_NUMBER;
    int             len_temp = 0;
    int             recv_len = 0;
    unsigned char   buf[COMM_BUFFER_MAX_LEN];
    UartFrame       *pFrame  = nullptr;
    unsigned char   errorBuff[COMM_BUFFER_MAX_LEN * 2];
    int             errorBuffOffset = 0;

    memset(buf, 0, sizeof(buf));
    fd_set rd;

    SysUtil::SetThreadName("UART_R");

    m_InitBarrier.wait();

    while (m_bRunning)
    {
        FD_ZERO(&rd);
        FD_SET(m_Usartfd, &rd);
        while (FD_ISSET(m_Usartfd, &rd))
//        while (true)
        {
//            printf("before select()\n");
            if (select(m_Usartfd + 1, &rd, nullptr, nullptr, nullptr) < 0)
            {
                perror("select error\n");
            }
            else
            {
                if (!m_bRunning) break;

//                printf("before read\n");
//                TICK(a2)
//                iReadLen = read(m_Usartfd, buf, 2048);
//                TOCK(a2, "read")
//                printf("iReadLen:%d\n", iReadLen);
//                sleep(1);

#if 0
                iReadLen = read(m_Usartfd, buf, 2048);
                printf("iReadLen:%d\n", iReadLen);
#else
                /*use status machine to control one commplete frame of data received*/
                if (state == WAIT_MAGIC_NUMBER) // 循环查找双字节的magic number
                {
                    iReadLen = read(m_Usartfd, buf, 2);

                    if (iReadLen == 1 && *(buf) == UART_FRAME_MAGIC_NUMBER >> 8)
                    {
                        state = WAIT_MAGIC_NUMBER_SECOND_BYTE;  // 已收到magic的第一个字节，继续等magic的第二个字节
                        recv_len += iReadLen;
                    }
                    else if (iReadLen == 2)
                    {
                        if (*(buf + 1) == (UART_FRAME_MAGIC_NUMBER & 0xFF)) // magic的第二个字节正确
                        {
                            if (*(buf) == UART_FRAME_MAGIC_NUMBER >> 8) // magic的第一个字节也正确
                            {
                                state = WAIT_LENGTH;  // 已收到完整的magic字段，等待接收length字段
                                recv_len += iReadLen;
                            }
                            else
                            {
                                state = WAIT_MAGIC_NUMBER_SECOND_BYTE;  // 只收到magic的第一个字节，继续等magic的第二个字节
                                *(buf) = *(buf + 1);
                                recv_len += 1;
                            }
                        }
                        else
                        {
//                            FR_PRINT_RED("UART", "Recv1: [%02X %02X]\n", *(buf), *(buf + 1));
                            memcpy(errorBuff + errorBuffOffset, buf, iReadLen);
                            errorBuffOffset += iReadLen;
                        }
                    }
                    else
                    {
//                        FR_PRINT_RED("UART", "Recv2: [%02X]\n", *(buf));
                        memcpy(errorBuff + errorBuffOffset, buf, iReadLen);
                        errorBuffOffset += iReadLen;
                    }
                }
                else if (state == WAIT_MAGIC_NUMBER_SECOND_BYTE) // 只收到magic的第一个字节，继续等magic的第二个字节
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 1);

                    if (iReadLen == 1 && *(buf + recv_len) == (UART_FRAME_MAGIC_NUMBER & 0xFF))
                    {
                        state = WAIT_LENGTH;  // 已收到完整的magic字段，等待接收length字段
                        recv_len += iReadLen;
                    }
                    else
                    {
                        recv_len = 0;
                        state    = WAIT_MAGIC_NUMBER;  // 收到的字节不符合magic number，重新开始接收magic numebr
//                        FR_PRINT_RED("UART", "Recv3: [%02X]\n", *(buf + recv_len));
                        memcpy(errorBuff + errorBuffOffset, buf, iReadLen);
                        errorBuffOffset += iReadLen;
                    }
                }
                else if (state == WAIT_LENGTH) // 等待接收2个字节的length
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 2);
                    if (iReadLen == 1)
                    {
                        state    = WAIT_LENGTH_SECOND_BYTE; // 只收到length的第一个字节，继续等length的第二个字节
                        len_temp = *(buf + recv_len);
                        recv_len += iReadLen;
                    }
                    else if (iReadLen == 2)
                    {
                        state    = WAIT_REST_FRAME_AND_DATA; // 按长度接收剩余的字节数据
                        len_temp = ((*(buf + recv_len + 1) << 8) & 0xFF00) + (*(buf + recv_len) & 0xFF);
                        recv_len += iReadLen;
                        if (len_temp > UART_FRAME_MAX_SIZE)
                        {
                            recv_len = 0;
                            state    = WAIT_MAGIC_NUMBER;
                            FR_PRINT_RED("UART", "Error: length:[%d] > %d\n", len_temp, UART_FRAME_MAX_SIZE);
                            memcpy(errorBuff + errorBuffOffset, buf, iReadLen);
                            errorBuffOffset += iReadLen;
                        }
                    }
                }
                else if (state == WAIT_LENGTH_SECOND_BYTE) // 只收到length的第一个字节，继续等length的第二个字节
                {
                    iReadLen = read(m_Usartfd, buf + recv_len, 1);
                    if (iReadLen == 1)
                    {
                        state = WAIT_REST_FRAME_AND_DATA;
                        len_temp += (((*(buf + recv_len)) << 8) & 0xFF00);
                        recv_len += iReadLen;
                        // at present, the max value of length is PROTO_LENGTH_MAX_NUMBER
                        if (len_temp > UART_FRAME_MAX_SIZE)
                        {
                            recv_len = 0;
                            state    = WAIT_MAGIC_NUMBER;
                            FR_PRINT_RED("UART", "Error: length:[%d] > %d\n", len_temp, UART_FRAME_MAX_SIZE);
                            memcpy(errorBuff + errorBuffOffset, buf, iReadLen);
                            errorBuffOffset += iReadLen;
                        }
                    }
                }
                else // 按长度接收剩余的字节数据
                {
                    static int last      = 0;
                    int        lenToRead = len_temp - last;
//                    if (lenToRead > 50)
//                    {
//                        lenToRead = 50;
//                    }
//                    if (last == 0)
//                    {
//                        printf("recv_len + last:%d\n", recv_len + last);
//                    }
                    iReadLen = read(m_Usartfd, buf + recv_len + last, lenToRead);
//                    printf("iReadLen:%d\n", iReadLen);
                    if (iReadLen < (len_temp - last) && iReadLen > 0)
                    {
                        last += iReadLen;
                    }
                    else
                    {
                        if (errorBuffOffset > 0)
                        {
                            PrintAsHexString(3, errorBuff, errorBuffOffset);
                            errorBuffOffset = 0;
                        }

                        recv_len += len_temp;
                        last  = 0;
                        state = WAIT_MAGIC_NUMBER;

//                        TICK(a)
//                        printf("\n=====================:%d\n", recv_len);
//                        for (int jj = 0; jj < recv_len; jj++)
//                        {
//                            printf("%02X ", buf[jj] & 0xFF);
//                        }
//                        printf("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333\n");
//                        printf("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333\n");
//                        printf("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333\n");
//                        printf("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333\n");
//                        printf("33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333\n");
//                        usleep(130000);
//                        TOCK(a, "printf")
                        pFrame = (UartFrame *) buf;

//                        printf("id:%u\n", pFrame->id);
//                        for (int i = 24; i < recv_len; i++)
//                        {
//                            if (buf[i] != 0x55)
//                            {
//                                printf(KRED "i:%d [0x%02X]\n" KRESET, i, buf[i]);
//                            }
//                        }

                        if (HandleRecvData(pFrame) != 0)
                        {
//                            FR_PRINT_RED("UART", "recv data parsed failed!\n");
                        }
                        else if (0)
                        {
                            PrintUartFrame("Recv", pFrame, true);
                        }
                        recv_len = 0;
                    }
                }
#endif
            }
        }
    }
}
