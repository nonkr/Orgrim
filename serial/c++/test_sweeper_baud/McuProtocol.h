#pragma once

#include <inttypes.h>

#define MCU_SERIAL_PORT_NAME    "/dev/ttyS1"
#define MCU_SERIAL_PORT_BAUD    460800

#define DEBUG_REG_SIZE            12

enum MCU_PACKAGE_TYPE : uint8_t
{
    H2M_GET_MCU_VERSION = 0x01,

    H2M_SET_SHUTDOWN       = 0x20,
    H2M_SET_RESTART        = 0x21,
    H2M_SET_DOMANT         = 0x22,
    H2M_SET_WAKEUP         = 0x23,
    H2M_SET_UPGRADE        = 0x24,
    H2M_SET_MOTION         = 0x25,
    H2M_SET_SYSTEM_PARAM   = 0x26,
    H2M_SET_SPEED_PARAM    = 0x27,
    H2M_SET_VIRTUAL_SENSOR = 0x28,

    H2M_SET_DEBUG = 0x2F,

    M2H_EMERGENCY_REG = 0xF0,
    M2H_FAST_REG      = 0xF1,
    M2H_SLOW_REG      = 0xF2,
    M2H_DEBUG_REG     = 0xFF
};

enum VIRTUAL_COLLISION_TYPE : uint8_t
{
    VCT_LEFT_COLLISION  = 0,
    VCT_RIGHT_COLLISION = 1,
    VCT_TOF_COLLISION   = 2,
    VCT_SLIP_ERROR      = 3,
    VCT_CURRENT_ERROR   = 4
};

#pragma pack(push, 1)

enum SensorId
{
    COLLISION_ID = 0,
    DROP_ID,
    IR_ID,
    PSD_ID,
    IMU_ID,
    ENCODER_ID,
    OPTICAL_FLOW_ID,
    MULTI_TOF_ID,
    PICK_UP_ID,
    LASER_ID,
    CHARGER_IR_ID,
    SENSOR_ID_END
};

//error status
#define COLLISION_ERROR_STATUS     1<<COLLISION_ID
#define DROP_ERROR_STATUS          1<<DROP_ID
#define IR_ERROR_STATUS            1<<IR_ID
#define PSD_ERROR_STATUS           1<<PSD_ID
#define IMU_ERROR_STATUS           1<<IMU_ID
#define ENCODER_ERROR_STATUS       1<<ENCODER_ID
#define OPTICAL_FLOW_ERROR_STATUS  1<<OPTICAL_FLOW_ID
#define MULTI_TOF_ERROR_STATUS     1<<MULTI_TOF_ID
#define PICK_UP_ERROR_STATUS       1<<PICK_UP_ID

enum MCU_MOTION_MODE : uint8_t
{
    EMERGENCY_MODE = 0, //紧急模式
    SPEED_MODE,         //遥控模式
    WALL_FOLLOW_MODE,   //沿边模式
    AUTO_CHARGE_MODE,    //自动回充
    EXECUTE_MODE,        //执行模式
    IDLE_MODE,            //空闲
    MCU_MOTION_MODE_NUM
};

//遥控子模式
enum SPEED_SUBMODE : uint8_t
{
    ENCODER_SPEED = 0,
    ENCODER_POSITOIN,
    IMU_SPEED,
    IMU_POSITION,
    LEFT_WHEEL_SPEED,
    RIGHT_WHEEL_SPEED
};

//沿边子模式
enum WALL_FOLLOW_SUBMODE : uint8_t
{
    LEFT = 0,
    RIGHT
};

//主按键
enum MAIN_KEY_STATE : uint8_t
{
    MKS_UP = 0,
    MKS_DOWN
};

//运动应用选择指令
struct Host2McuMotionReg
{
    uint8_t mode    = SPEED_MODE;
    uint8_t bEnable = 1;
    uint8_t submode = IMU_SPEED;
};

//遥控数据通道
struct Host2McuSpeedParamReg
{
    int32_t v     = 0;      //线速度
    int32_t w     = 0;      //角速度
    int32_t dis   = 0;    //距离
    int32_t angle = 0;    //角度
    int32_t vAcc  = 1000;//线加速度
    int32_t wAcc  = 10000;//角加速度
};

//虚拟传感器数据通道
struct Host2McuSensorReg
{
    uint8_t bSlowDown      = 0;        //靠近障碍物减速
    uint8_t bCollision     = 0;        //碰撞
    int16_t collisionAngle = 0; //碰撞角度
    int16_t leftPsdDis     = -1;    //左虚拟沿边距离
    int16_t rightPsdDis    = -1;    //右虚拟沿边距离
    int16_t curPsdPidP     = 0;        //当前沿边距离P
    int16_t curPsdPidD     = 0;        //当前沿边距离D
};

//下行debug数据通道
struct Host2McuDebugReg
{
    uint32_t triggerBits           = ((1 << DEBUG_REG_SIZE) - 1);
    int16_t  debug[DEBUG_REG_SIZE] = {0};
};

//10ms一个快包
struct Mcu2HostFastReg
{
    //里程计积分量
    int32_t  opticalFlowX = 0;   //光流x
    int32_t  opticalFlowY = 0;   //光流y
    int32_t  leftPos      = 0;        //左轮
    int32_t  rightPos     = 0;       //右轮
    int32_t  yawPos       = 0;         //水平面角度积分
    //单点tof，psd
    uint16_t psdLeft      = 0;
    uint16_t psdRight     = 0;
    uint16_t psdFront     = 0;
    //底层解算的位姿
    int32_t  odoX         = 0;
    int32_t  odoY         = 0;
    int32_t  odoPhi       = 0;
    //倾斜角
    int32_t  pitch        = 0;
    int32_t  roll         = 0;
    //实测速度
    int32_t  v            = 0;
    int32_t  w            = 0;
    //lidar tof
    uint16_t lidarLeft    = {0};
    uint16_t lidarRight   = {0};
};

//50ms一个慢包
struct Mcu2HostSlowReg
{
    //底层状态机
    uint8_t motionMode    = 0;
    uint8_t motionSubmode = 0;

    //传感器反馈
    uint8_t  collision             = 0;
    uint16_t ir                    = 0;
    uint8_t  drop                  = 0;
    uint16_t opticalCollisionAngle = 0; //最高位表示是否有光学碰撞
    uint8_t  pickUpStatus          = 0;
    uint8_t  chargeStatus          = 0;
    uint32_t enableStatus          = (((uint64_t) 1 << 32) - 1);

    //打滑检测
    uint8_t leftSlip  = 0;
    uint8_t rightSlip = 0;

    //错误码反馈
    uint32_t errorStatus = 0;

    //按钮
    MAIN_KEY_STATE key = MKS_UP;

    uint8_t  currentStatus = 0;
    uint32_t chargerIrInfo = 0;
};

struct Mcu2HostDebugReg
{
    int16_t debug[DEBUG_REG_SIZE] = {0};
};

#pragma pack(pop)
