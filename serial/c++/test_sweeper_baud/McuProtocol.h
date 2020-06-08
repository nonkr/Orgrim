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
    EMERGENCY_MODE = 0, //����ģʽ
    SPEED_MODE,         //ң��ģʽ
    WALL_FOLLOW_MODE,   //�ر�ģʽ
    AUTO_CHARGE_MODE,    //�Զ��س�
    EXECUTE_MODE,        //ִ��ģʽ
    IDLE_MODE,            //����
    MCU_MOTION_MODE_NUM
};

//ң����ģʽ
enum SPEED_SUBMODE : uint8_t
{
    ENCODER_SPEED = 0,
    ENCODER_POSITOIN,
    IMU_SPEED,
    IMU_POSITION,
    LEFT_WHEEL_SPEED,
    RIGHT_WHEEL_SPEED
};

//�ر���ģʽ
enum WALL_FOLLOW_SUBMODE : uint8_t
{
    LEFT = 0,
    RIGHT
};

//������
enum MAIN_KEY_STATE : uint8_t
{
    MKS_UP = 0,
    MKS_DOWN
};

//�˶�Ӧ��ѡ��ָ��
struct Host2McuMotionReg
{
    uint8_t mode    = SPEED_MODE;
    uint8_t bEnable = 1;
    uint8_t submode = IMU_SPEED;
};

//ң������ͨ��
struct Host2McuSpeedParamReg
{
    int32_t v     = 0;      //���ٶ�
    int32_t w     = 0;      //���ٶ�
    int32_t dis   = 0;    //����
    int32_t angle = 0;    //�Ƕ�
    int32_t vAcc  = 1000;//�߼��ٶ�
    int32_t wAcc  = 10000;//�Ǽ��ٶ�
};

//���⴫��������ͨ��
struct Host2McuSensorReg
{
    uint8_t bSlowDown      = 0;        //�����ϰ������
    uint8_t bCollision     = 0;        //��ײ
    int16_t collisionAngle = 0; //��ײ�Ƕ�
    int16_t leftPsdDis     = -1;    //�������ر߾���
    int16_t rightPsdDis    = -1;    //�������ر߾���
    int16_t curPsdPidP     = 0;        //��ǰ�ر߾���P
    int16_t curPsdPidD     = 0;        //��ǰ�ر߾���D
};

//����debug����ͨ��
struct Host2McuDebugReg
{
    uint32_t triggerBits           = ((1 << DEBUG_REG_SIZE) - 1);
    int16_t  debug[DEBUG_REG_SIZE] = {0};
};

//10msһ�����
struct Mcu2HostFastReg
{
    //��̼ƻ�����
    int32_t  opticalFlowX = 0;   //����x
    int32_t  opticalFlowY = 0;   //����y
    int32_t  leftPos      = 0;        //����
    int32_t  rightPos     = 0;       //����
    int32_t  yawPos       = 0;         //ˮƽ��ǶȻ���
    //����tof��psd
    uint16_t psdLeft      = 0;
    uint16_t psdRight     = 0;
    uint16_t psdFront     = 0;
    //�ײ�����λ��
    int32_t  odoX         = 0;
    int32_t  odoY         = 0;
    int32_t  odoPhi       = 0;
    //��б��
    int32_t  pitch        = 0;
    int32_t  roll         = 0;
    //ʵ���ٶ�
    int32_t  v            = 0;
    int32_t  w            = 0;
    //lidar tof
    uint16_t lidarLeft    = {0};
    uint16_t lidarRight   = {0};
};

//50msһ������
struct Mcu2HostSlowReg
{
    //�ײ�״̬��
    uint8_t motionMode    = 0;
    uint8_t motionSubmode = 0;

    //����������
    uint8_t  collision             = 0;
    uint16_t ir                    = 0;
    uint8_t  drop                  = 0;
    uint16_t opticalCollisionAngle = 0; //���λ��ʾ�Ƿ��й�ѧ��ײ
    uint8_t  pickUpStatus          = 0;
    uint8_t  chargeStatus          = 0;
    uint32_t enableStatus          = (((uint64_t) 1 << 32) - 1);

    //�򻬼��
    uint8_t leftSlip  = 0;
    uint8_t rightSlip = 0;

    //�����뷴��
    uint32_t errorStatus = 0;

    //��ť
    MAIN_KEY_STATE key = MKS_UP;

    uint8_t  currentStatus = 0;
    uint32_t chargerIrInfo = 0;
};

struct Mcu2HostDebugReg
{
    int16_t debug[DEBUG_REG_SIZE] = {0};
};

#pragma pack(pop)
