#pragma once

#include <inttypes.h>

#define MCU_SERIAL_PORT_NAME    "/dev/ttyS1"
#define MCU_SERIAL_PORT_BAUD    (921600)

#define UART_PROTOCOL_VERSION       "V0.0.9 build 200720"

#define UART_HOST2MCU_MAX_SIZE (512)        // 计算板发送到控制板时每次写入串口设备的最大数据量

#define UART_FRAME_MAGIC_NUMBER              (0xAAAA)
#define UART_FRAME_SIZE_BEFORE_WITH_LENGTH   (4)
#define UART_FRAME_SIZE_AFTER_LENGTH         (16)
#define UART_FRAME_HEADER_SIZE               (UART_FRAME_SIZE_BEFORE_WITH_LENGTH + UART_FRAME_SIZE_AFTER_LENGTH)
#define UART_FRAME_DATA_MAX_SIZE             (324)   // 等于sizeof(UartMcu2HostLidarReg)
#define UART_FRAME_MAX_SIZE                  (UART_FRAME_DATA_MAX_SIZE + UART_FRAME_HEADER_SIZE)

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)
#define BITLEN(a) (sizeof(a) / sizeof(char))

#pragma pack(push, 1)

// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |          Magic Number         |            Length             |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                              ID                               |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |               |     |F|F|D|A|N|                               |
// |    Command    | Opt |G|R|U|C|A|        Sequence Number        |
// |               |     |D|G|P|K|K|                               |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |      Acknowledgment Number    |  Shadow Acknowledgment Number |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |   Fragment Sequence Number    |            Checksum           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                             data                              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct UartFrame
{
    uint16_t magic;         // Magic Number
    uint16_t length;        // 包长(length字段之后的所有长度)
    uint32_t id;            // ID(用于跟踪丢帧率)
    uint8_t  command;       // Command Type
    uint8_t  options;       // need ACK, ACK, Duplicate, Fragment, Fragment done
    uint16_t sn;            // Sequence Number
    uint16_t ack;           // Acknowledgment Number
    uint16_t sack;          // Shadow Acknowledgment Number
    uint16_t frag_sn;       // Fragment Sequence Number
    uint16_t checksum;      // Checksum
    uint8_t  data[0];
};

// 协议命令类型
enum MCU_COMMAND_TYPE : uint8_t
{
    MCT_H2M_MCU_FIRMWARE_VERSION = 0x01,    // 固件版本查询
    MCT_H2M_MCU_PROTOCOL_VERSION = 0x02,    // 协议版本查询

    MCT_H2M_SHUTDOWN            = 0x20,     // 关机
    MCT_H2M_RESTART             = 0x21,     // 重启
    MCT_H2M_SLEEP               = 0x22,     // 休眠
    MCT_H2M_SET_FAN_MODE        = 0x23,     // 设置风机模式
    MCT_H2M_SET_EDGE_BRUSH_MODE = 0x24,     // 设置边刷模式
    MCT_H2M_SET_LED_MODE        = 0x25,     // 设置LED模式
    MCT_H2M_SET_MOTION          = 0x26,     // 设置运动模式
    MCT_H2M_SET_SYSTEM_PARAM    = 0x27,     // 设置系统参数
    MCT_H2M_SET_SPEED_PARAM     = 0x28,     // 设置遥控参数
    MCT_H2M_SET_VIRTUAL_SENSOR  = 0x29,     // 虚拟传感器

    MCT_H2M_UPDATE_SLAM_POSE  = 0x30,       // 位姿更新
    MCT_H2M_SET_TOF_SENSOR    = 0x31,       // 大ToF障碍物信息
    MCT_H2M_CHARGER_INFO      = 0x32,       // 充电座识别信息
    MCT_HEARTBEAT             = 0x33,       // 心跳
    MCT_H2M_SET_LIDAR_CAPTURE = 0x34,       // 设置雷达采集模式
    MCT_H2M_SET_TIMESTAMP     = 0x35,       // 设置时间戳（从1970年01月01日00时00分00秒起至现在的总微秒数）

    MCT_H2M_SET_DEBUG     = 0x41,           // 下行Debug通道
    MCT_H2M_DEBUG_COMMAND = 0x42,           // 下行Debug命令

//    MCT_M2H_EMERGENCY_REG = 0xF0,         // 应急上传
    MCT_M2H_FAST_REG  = 0xF1,               // 快包上传
    MCT_M2H_SLOW_REG  = 0xF2,               // 慢包上传
    MCT_M2H_LIDAR_REG = 0xF3,               // 雷达数据上传
    MCT_M2H_EVENT_REG = 0xF4,               // 事件上传
    MCT_M2H_DEBUG_REG = 0xFF                // 上行Debug通道
};

struct UartPose2D
{
    float x    = 0;
    float y    = 0;
    float mPhi = 0;
};

struct UartLidarUnit
{
    UartPose2D sweeperPose;      // 雷达采样时的扫地机位姿
    int16_t    leftLidar  = 0;        // 采样时的左雷达数据
    int16_t    rightLidar = 0;       // 采样时的右雷达数据
};

#define MCU_LIDAR_MAX_UNIT_SIZE (1000)
#define MCU_LIDAR_MAX_SIZE (MCU_LIDAR_MAX_UNIT_SIZE * sizeof(UartLidarUnit))
#define MCU_LIDAR_ONE_FRAME_MAX_UINT_SIZE (20)

// MCT_M2H_LIDAR_REG（上行雷达数据包）
struct UartMcu2HostLidarReg
{
    uint8_t       totalNo   = 0;           // 总共有多少个包
    uint8_t       currentNo = 0;         // 当前是第几个包（从1开始计数）
    uint16_t      count     = 0;             // 雷达数据的unit个数
    UartLidarUnit lidar[MCU_LIDAR_ONE_FRAME_MAX_UINT_SIZE];      // 雷达数据
};
struct Mcu2HostLidarReg
{
    uint16_t      count = 0;             // 雷达数据的unit个数
    UartLidarUnit lidar[0];                 // 雷达数据
};

#pragma pack(pop)
