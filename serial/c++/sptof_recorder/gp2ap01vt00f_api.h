#ifndef _GP2AP01VT00F_API_H_
#define _GP2AP01VT00F_API_H_

//#include "stm32f10x.h"
#include "math.h"
//#include "I2C.h"

#ifdef __cplusplus
extern "C" {
#endif

#define true 1
#define false 0
typedef char           u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed short   s16;

int seq_i2c_write(u8 slave_addr, u8 reg_addr, u8 *data, u8 number);

int seq_i2c_read(u8 slave_addr, u8 reg_addr, u8 *data, u8 number);


/* software version */
#define SOFT_REVISION                "gp2ap01vt00f_v109"

#ifndef M_PI
#define M_PI (3.141593)
#endif

/* operation mode */
#define SINGLE_MODE                    0
#define CONTINUOUS_MODE                1

#define INTERRUPT_MODE                0
#define POLLING_MODE                1

/* output mode */
#define COMMANDMODE                    0    /* terminal */
#define OUTPUT2TERMINAL                1    /* terminal */
#define OUTPUT2WIN                    3    /* windows app */

/* error status */
#define VALID_DATA                    0
#define SIGNAL_ERROR                1
#define CLOSE_RANGE_ERROR            2
#define WAF_ERROR                    3
#define HARDWARE_ERROR                4
#define SOFT_WAF_ERROR                5

/* factory calibration */
#define OFFSET_CALIB_DISTANCE        100    /* 100mm */
#define XTALK_CALIB_DISTANCE        600    /* 600mm */

/* signal check */
#define LOW_RET_SIG                    349
#define LOW_AMB_COEFF                0.2
#define LOW_XTALK_COEFF            2.7
#define MAX                            3000


extern const u8 slave_addr;


/* struct declaration */
struct gp2ap01vt00f_data
{
    u8 operation_mode;        /* continuous/single mode */
    u8 output_mode;        /* terminal/win app */
    u8 get_mode;            /* interrupt/polling */

    /* range parameters */
    u32 range_result;
    u32 range_result2;
    u32 ret_sig;
    u32 ret_sig2;
    u32 ret_amb;
    u8  reg01h;
    u8  reg10h;
    u32 c1;
    u32 c2;

    u16 range_mm;
    u8  range_status;

    /* offset calibration with panel */
    s16 offset_with_panel;
    s16 OffsetCalDistanceAsInt;

    /* crosstalk calibration */
    s16 xTalkCalDistanceAsInt;
    u8  XTalkCompensationEnable;
    u16 xTalkRetSig;

    /* median */
    u8  num_median;
    u16 data_median_temp[3];
    u32 ret_sig_pre;
    u8  range_status_pre;

    /* average */
    u8    num_measure;
    float range_mm_ave;
    float ret_sig_ave;
    float ret_amb_ave;
    float range_mm_ave_data[10];
    float ret_sig_ave_data[10];
    float ret_amb_ave_data[10];
};

void initGPparams(struct gp2ap01vt00f_data *);

void DataInit(struct gp2ap01vt00f_data *);

void StartMeasurement(struct gp2ap01vt00f_data *);

void StopMeasurement(struct gp2ap01vt00f_data *);

void ReadRangeData(struct gp2ap01vt00f_data *);

void CalculateDistance(struct gp2ap01vt00f_data *);

void GetRangingMeasurementData(struct gp2ap01vt00f_data *);

void GP2AP01VT_apply_xtalk_compensation(struct gp2ap01vt00f_data *);

void SetDeviceMode(struct gp2ap01vt00f_data *, u8, u8);


#ifdef __cplusplus
}
#endif

#endif //_GP2AP01VT00F_API_H_



