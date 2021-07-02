#include <cstring>
#include "McuComm.h"
#include "SerialPort.h"
#include "McuProtocol.h"
#include "../../../common/PrintUtil.h"
#include "../../../common/TimeUtil.h"

using namespace std;

static SerialPort *g_pMcuSerialPort = nullptr;
static locker     g_SerialPortLocker;
static bool       g_bReplies[256]   = {false};
static uint8_t    g_ReplyCodes[256] = {0};
static string     g_McuFirmwareVersion;
static string     g_McuProtocolVersion;

#define FR_PRINT_G0(tag, ...) printf(KGREEN "[%s:%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__FILENAME__,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_G1(tag, ...) printf(KGREEN "[%d] " FIRST_ARG(__VA_ARGS__) KRESET ,__LINE__ REST_ARGS(__VA_ARGS__))
#define FR_PRINT_G2(tag, ...)  printf(KGREEN FIRST_ARG(__VA_ARGS__) KRESET REST_ARGS(__VA_ARGS__))

static void RecvCallBack(UartFrame *pFrame)
{
    MCU_COMMAND_TYPE command    = (MCU_COMMAND_TYPE) pFrame->command;
    int              len        = pFrame->length - UART_FRAME_SIZE_AFTER_LENGTH;
    static uint32_t  id         = 0;
    static uint16_t  totalLost  = 0;
    static uint32_t  totalFrame = 0;

    {
        totalFrame++;
        if (id != 0 && id + 1 != pFrame->id)
        {
            totalLost += pFrame->id - id - 1;
//            FR_PRINT_RED("uart", "uart lost frame between %u and %u, total lost:%u(%.2f%%)\n", id, pFrame->id,
//                         totalLost,
//                         totalLost * 1.0 / totalFrame);
        }
        id = pFrame->id;
        FR_PRINT_GREEN("uart", "id:%u command:0x%02X\n", pFrame->id, pFrame->command);
    }

    switch (command)
    {
        case MCT_M2H_LIDAR_REG:
        {
            if (len != sizeof(UartMcu2HostLidarReg))
            {
                FR_PRINT_RED("uart", "Mcu2HostLidarReg len error, in %d, should be %zu\n", len,
                             sizeof(UartMcu2HostLidarReg));
                break;
            }
//            UartMcu2HostLidarReg *plidarReg = (UartMcu2HostLidarReg *) pFrame->data;
//            memcpy(&lidarReg, pFrame->data, sizeof(UartMcu2HostLidarReg));
//            TICK(a)
//            carrier::OnNewLidarReg(*plidarReg);
//            TOCK(a, "OnNewLidarReg")
        }
            break;

        default:
            FR_PRINT_RED("uart", "Received unknown command:0x%02X\n", command);
            break;
    }
//    FR_PRINT_GREEN("carrier", "Lidar count:%d\n", g_ReplyCodes[256]);
    g_bReplies[command]   = true;
    g_ReplyCodes[command] = pFrame->data[0];
    //LOGD("receive type = [%02x]", (int)type);
}

bool mcu_comm::Init()
{
    g_SerialPortLocker.lock();
    if (g_pMcuSerialPort)
    {
        FR_PRINT_RED("mcu", "SerialPort is already inited\n");
        g_SerialPortLocker.unlock();
        return true;
    }

    g_pMcuSerialPort = new SerialPort(MCU_SERIAL_PORT_NAME, MCU_SERIAL_PORT_BAUD);
    bool ret = g_pMcuSerialPort->Init(RecvCallBack, true);
    g_SerialPortLocker.unlock();

    return ret;
}

bool mcu_comm::DeInit()
{
    g_SerialPortLocker.lock();
    if (!g_pMcuSerialPort)
    {
        FR_PRINT_RED("mcu", "SerialPort is not inited\n");
        g_SerialPortLocker.unlock();
        return true;
    }

    g_pMcuSerialPort->DeInit();
    delete g_pMcuSerialPort;
    g_pMcuSerialPort = nullptr;
    g_SerialPortLocker.unlock();

    return true;
}

