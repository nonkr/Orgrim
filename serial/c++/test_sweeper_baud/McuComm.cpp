#include <cstring>
#include "McuComm.h"
#include "SerialPort.h"
#include "McuProtocol.h"
#include "../../../common/PrintUtil.h"
#include "../../../common/TimeUtil.h"

using namespace std;

static SerialPort g_McuSerialPort(MCU_SERIAL_PORT_NAME, MCU_SERIAL_PORT_BAUD);
static bool       g_bReplies[256]   = {false};
static uint8_t    g_ReplyCodes[256] = {0};
static string     g_McuVersion;

static void RecvCallBack(SerialPortData *buf)
{
    MCU_PACKAGE_TYPE type = (MCU_PACKAGE_TYPE) buf->type;
    int              len  = buf->len;

#if 0
    switch (type)
    {
        case H2M_GET_MCU_VERSION:
            FR_PRINT_YELLOW("UART", "recv: H2M_GET_MCU_VERSION\n");
            break;
        case H2M_SET_SHUTDOWN:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_SHUTDOWN\n");
            break;
        case H2M_SET_RESTART:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_RESTART\n");
            break;
        case H2M_SET_DOMANT:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_DOMANT\n");
            break;
        case H2M_SET_WAKEUP:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_WAKEUP\n");
            break;
        case H2M_SET_UPGRADE:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_UPGRADE\n");
            break;
        case H2M_SET_MOTION:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_MOTION\n");
            break;
        case H2M_SET_SYSTEM_PARAM:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_SYSTEM_PARAM\n");
            break;
        case H2M_SET_SPEED_PARAM:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_SPEED_PARAM\n");
            break;
        case H2M_SET_VIRTUAL_SENSOR:
            FR_DURATION_AVG(3, "# H2M_SET_VIRTUAL_SENSOR");
//            FR_PRINT_YELLOW("UART", "recv: H2M_SET_VIRTUAL_SENSOR\n");
            break;
        case H2M_SET_DEBUG:
            FR_PRINT_YELLOW("UART", "recv: H2M_SET_DEBUG\n");
            break;
        case M2H_EMERGENCY_REG:
            FR_PRINT_YELLOW("UART", "recv: M2H_EMERGENCY_REG\n");
            break;
        case M2H_FAST_REG:
            FR_DURATION_AVG(1, "# M2H_FAST_REG");
//            FR_PRINT_GREEN("UART", "recv: M2H_FAST_REG\n");
            break;
        case M2H_SLOW_REG:
            FR_DURATION_AVG(2, "# M2H_SLOW_REG");
//            FR_PRINT_BLUE("UART", "recv: M2H_SLOW_REG\n");
            break;
        case M2H_DEBUG_REG:
            FR_PRINT_YELLOW("UART", "recv: M2H_DEBUG_REG\n");
            break;
        default:
            FR_PRINT_YELLOW("UART", "recv: unknown command type\n");
    }
#endif

    switch (type)
    {
        //State reports
        case M2H_FAST_REG:
        {
            if (len != sizeof(Mcu2HostFastReg))
            {
                //LOGD("Mcu2HostFastReg len error, len = %d, req = %d", len, sizeof(Mcu2HostFastReg));
            }
            Mcu2HostFastReg fastReg;
            memcpy(&fastReg, buf->data, sizeof(Mcu2HostFastReg));
//            carrier::OnNewMcuFastReg(fastReg);

            static int lastV = fastReg.v;
            if (lastV != 88 && fastReg.v == 88)
            {
                g_McuSerialPort.ResetCounts();
            }
        }
            break;
        case M2H_EMERGENCY_REG:
        {
            SerialPortData buf;
            buf.len  = 0;
            buf.type = M2H_EMERGENCY_REG;
            g_McuSerialPort.AddToSendBuf(&buf);
        }
        case M2H_SLOW_REG:
        {
            if (len != sizeof(Mcu2HostSlowReg))
            {
//                LOGD("Mcu2HostSlowReg len error, len = %d, req = %d", len, sizeof(Mcu2HostSlowReg));
            }
            Mcu2HostSlowReg slowReg;
            memcpy(&slowReg, buf->data, sizeof(Mcu2HostSlowReg));
//            carrier::OnNewMcuSlowReg(slowReg);
        }
            break;
        case M2H_DEBUG_REG:
        {
            if (len != sizeof(Mcu2HostDebugReg))
            {
//                LOGD("Mcu2HostDebugReg len error, len = %d, req = %d", len, sizeof(Mcu2HostDebugReg));
            }
            Mcu2HostDebugReg debugReg;
            memcpy(&debugReg, buf->data, sizeof(Mcu2HostDebugReg));
//            carrier::OnNewMcuDebugReg(debugReg);
        }
            break;

            //Get replys
        case H2M_GET_MCU_VERSION:
        {
            g_McuVersion = string((char *) buf->data);
//            LOGD("Get Mcu Version = %s", buf->data);
        }
            break;

            //Set replys
        case H2M_SET_SHUTDOWN:
        case H2M_SET_RESTART:
        case H2M_SET_DOMANT:
        case H2M_SET_WAKEUP:
        case H2M_SET_UPGRADE:
        case H2M_SET_MOTION:
        case H2M_SET_SYSTEM_PARAM:
        case H2M_SET_SPEED_PARAM:
        case H2M_SET_VIRTUAL_SENSOR:
        case H2M_SET_DEBUG:
        {
            //todo
            //if (type == H2M_SET_VIRTUAL_SENSOR ||
            //	type == H2M_SET_SPEED_PARAM)
            //{
            //	LOGD("receive type = [%02x], code = [%02x]", (int)type, (int)buf->data[0]);
            //}
        }
            break;

        default:
//            LOGD("Unknown receive data type = %d", type);
            break;
    }

    g_bReplies[type]   = true;
    g_ReplyCodes[type] = buf->data[0];
    //LOGD("receive type = [%02x]", (int)type);
}

bool mcu_comm::Init()
{
    return g_McuSerialPort.Init(RecvCallBack, true);
}

std::string mcu_comm::GetMcuVertion()
{
    SerialPortData buf;
    buf.len  = 0;
    buf.type = H2M_GET_MCU_VERSION;
    g_McuSerialPort.AddToSendBuf(&buf);

    while (!g_bReplies[buf.type])
    {
        Sleep(5);
//        LOGD("Wait For Mcu Reply ...");
    }
    g_bReplies[buf.type] = false;
//    LOGD("Mcu Reply Code = [%02x]", g_ReplyCodes[buf.type]);

    return g_McuVersion;
}

bool mcu_comm::SetShutdown(uint16_t timeDelayMs)
{
    static uint8_t dataBuf[sizeof(timeDelayMs)];

    SerialPortData buf;
    buf.len  = sizeof(timeDelayMs);
    buf.type = H2M_SET_SHUTDOWN;
    buf.data = dataBuf;
    memcpy(dataBuf, &timeDelayMs, sizeof(timeDelayMs));
    g_McuSerialPort.AddToSendBuf(&buf);

    while (!g_bReplies[buf.type])
    {
        Sleep(5);
//        LOGD("Wait For Mcu Reply ...");
    }
    g_bReplies[buf.type] = false;
//    LOGD("Mcu Reply Code = [%02x]", g_ReplyCodes[buf.type]);
    return true;
}

bool mcu_comm::SetRestart(uint16_t timeDelayMs)
{
    static uint8_t dataBuf[sizeof(timeDelayMs)];

    SerialPortData buf;
    buf.len  = sizeof(timeDelayMs);
    buf.type = H2M_SET_RESTART;
    buf.data = dataBuf;
    memcpy(dataBuf, &timeDelayMs, sizeof(timeDelayMs));
    g_McuSerialPort.AddToSendBuf(&buf);

    while (!g_bReplies[buf.type])
    {
        usleep(500);
//        LOGD("Wait For Mcu Reply ...");
    }
    g_bReplies[buf.type] = false;
//    LOGD("Mcu Reply Code = [%02x]", g_ReplyCodes[buf.type]);
    return true;
}

bool mcu_comm::SetDormant(uint16_t timeDelayMs)
{
    static uint8_t dataBuf[sizeof(timeDelayMs)];

    SerialPortData buf;
    buf.len  = sizeof(timeDelayMs);
    buf.type = H2M_SET_DOMANT;
    buf.data = dataBuf;
    memcpy(dataBuf, &timeDelayMs, sizeof(timeDelayMs));
    g_McuSerialPort.AddToSendBuf(&buf);

    while (!g_bReplies[buf.type])
    {
        Sleep(5);
//        LOGD("Wait For Mcu Reply ...");
    }
    g_bReplies[buf.type] = false;
//    LOGD("Mcu Reply Code = [%02x]", g_ReplyCodes[buf.type]);
    return true;
}

bool mcu_comm::SetWakeup()
{
    //TODO
    return true;
}

bool mcu_comm::SetUpgrade()
{
    //TODO
    return true;
}

bool mcu_comm::SetMotionReg(const Host2McuMotionReg &motionReg)
{
    static uint8_t dataBuf[sizeof(Host2McuMotionReg)];

    SerialPortData buf;
    buf.len  = sizeof(motionReg);
    buf.type = H2M_SET_MOTION;
    buf.data = dataBuf;
    memcpy(dataBuf, &motionReg, sizeof(motionReg));

#if 0
    while (!g_bReplies[buf.type])
    {
        g_McuSerialPort.AddToSendBuf(&buf);
        Sleep(10);
        LOGD("Wait For Mcu Reply ...");

    }
#else
    g_McuSerialPort.AddToSendBuf(&buf);
#endif

    //g_bReplies[buf.type] = false;
    //LOGD("Mcu Reply Code = [%02x]", (int)g_ReplyCodes[buf.type]);
    return true;
}

bool mcu_comm::SetSpeedParamReg(const Host2McuSpeedParamReg &speedReg)
{
    static uint8_t dataBuf[sizeof(Host2McuSpeedParamReg)];

    SerialPortData buf;
    buf.len  = sizeof(speedReg);
    buf.type = H2M_SET_SPEED_PARAM;
    buf.data = dataBuf;
    memcpy(dataBuf, &speedReg, sizeof(speedReg));
    g_McuSerialPort.AddToSendBuf(&buf);

    return true;
}


bool mcu_comm::SetSensorReg(const Host2McuSensorReg &sensorReg)
{
    static uint8_t dataBuf[sizeof(Host2McuSensorReg)];

    SerialPortData buf;
    buf.len  = sizeof(sensorReg);
    buf.type = H2M_SET_VIRTUAL_SENSOR;
    buf.data = dataBuf;
    memcpy(dataBuf, &sensorReg, sizeof(sensorReg));

    g_McuSerialPort.AddToSendBuf(&buf);
//    FR_PRINT_YELLOW("mcu_comm", "Send: H2M_SET_VIRTUAL_SENSOR\n");
    return true;
}

bool mcu_comm::SetHostDebugReg(const Host2McuDebugReg &debugReg)
{
    static uint8_t dataBuf[sizeof(Host2McuDebugReg)];

    SerialPortData buf;
    buf.len  = sizeof(debugReg);
    buf.type = H2M_SET_DEBUG;
    buf.data = dataBuf;
    memcpy(dataBuf, &debugReg, sizeof(debugReg));

    g_McuSerialPort.AddToSendBuf(&buf);
    return true;
}
