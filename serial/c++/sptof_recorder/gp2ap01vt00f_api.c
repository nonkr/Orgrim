/////////////////////////////////////////////////////////////////////////
//Copyright (C) 2017 SHARP All Rights Reserved.
//This program is for modeling purposes only, and is not guaranteed.
//This program is API of the GP2AP01VT00F m_Psensor.
//Author : Isamu Kawabe <kawabe.isamu@sharp.co.jp>
/////////////////////////////////////////////////////////////////////////

#include "gp2ap01vt00f_api.h"

//从设备地址，8位地址的最低位为读写标志位，写操作为0，读操作为1
#define SLAVE_ADDRESS   0x29   //;         //( 0x29 << 1 ) | X, write_mode:X=0x52, read_mode:X=0x53
const u8 slave_addr = SLAVE_ADDRESS;


#ifdef PIC18F26K22
//#define DELAY_FUNC(value) delay_ms(value)
#define DEBUG_MSG_0(msg) \
  do { \
    printf(#msg); \
    printf("\r\n"); \
  } while(0);
#define DEBUG_MSG_1(msg, a) \
  do { \
    printf(#msg); \
    printf("%ld\r\n",a); \
  } while(0);
#define DEBUG_MSG_1F(msg, a) \
  do { \
    printf(#msg); \
    printf("%f\r\n",a); \
  } while(0);
#define DEBUG_MSG_1HEX(msg, a) \
  do { \
    printf(#msg); \
    printf("%02LX\r\n",a); \
  } while(0);
#endif

#ifdef ARDUINO_UNO
//#define DELAY_FUNC(value) delay(value)
#define DEBUG_MSG_0(msg) \
  do { \
    Serial.print(#msg); \
    Serial.println(""); \
  } while(false);
#define DEBUG_MSG_1(msg, a) \
  do { \
    Serial.print(#msg); \
    Serial.print(a); \
    Serial.println(""); \
  } while(0);
#define DEBUG_MSG_1F(msg, a) \
  do { \
    Serial.print(#msg); \
    Serial.print(a); \
    Serial.println(""); \
  } while(0);
#define DEBUG_MSG_1HEX(msg, a) \
  do { \
    Serial.print(#msg); \
    Serial.print(a,HEX); \
    Serial.println(""); \
  } while(0);
#endif


int seq_i2c_write(u8 slave_addr, u8 reg_addr, u8 *data, u8 number)
{

}

int seq_i2c_read(u8 slave_addr, u8 reg_addr, u8 *data, u8 number)
{

}


u16 func_median(u16 data[])
{
	u8  a, b;
	u16 temp, median;
	u16 sub_data[3];
	u8  num = 3;

	for (a = 0; a < num; a++)
	{
		sub_data[a] = data[a];
	}

	for (a = 0; a < num - 1; a++)
	{
		for (b = num - 1; b > a; b--)
		{
			if (sub_data[b - 1] > sub_data[b])
			{
				temp = sub_data[b];
				sub_data[b]     = sub_data[b - 1];
				sub_data[b - 1] = temp;
			}
		}
	}

	median = sub_data[num / 2];

	return median;
}

/* initialize parameters */
void initGPparams(struct gp2ap01vt00f_data *p_gp)
{
	p_gp->range_result           = 0;
	p_gp->range_result2          = 0;
	p_gp->ret_sig                = 0;
	p_gp->ret_sig2               = 0;
	p_gp->ret_amb                = 0;
	p_gp->range_mm               = 0;
	p_gp->range_mm_ave           = 0;
	p_gp->ret_sig_ave            = 0;
	p_gp->ret_amb_ave            = 0;
	p_gp->c1                     = 0;
	p_gp->c2                     = 0;
	p_gp->offset_with_panel      = 0;
	p_gp->xTalkRetSig            = 0;
	p_gp->OffsetCalDistanceAsInt = OFFSET_CALIB_DISTANCE;
	p_gp->xTalkCalDistanceAsInt  = XTALK_CALIB_DISTANCE;
	p_gp->num_measure            = 10;
	p_gp->num_median             = 0;
	p_gp->data_median_temp[0] = 0;
	p_gp->data_median_temp[1] = 0;
	p_gp->data_median_temp[2] = 8888;
	p_gp->range_status_pre = 0;
	p_gp->ret_sig_pre      = 0;
}

void SetDeviceMode(struct gp2ap01vt00f_data *p_gp, u8 mode1, u8 mode2)
{
	/* operation mode, SINGLE_MODE / CONTINUOUS_MODE*/
	p_gp->operation_mode = mode1;

	/* get mode, INTERRUPT_MODE / POLLING_MODE */
	p_gp->get_mode = mode2;

}

/* Sesor initialization */
void DataInit(struct gp2ap01vt00f_data *p_gp)
{
	initGPparams(p_gp);
}

void StartMeasurement(struct gp2ap01vt00f_data *p_gp)
{
	u8 wdata;

	if (p_gp->operation_mode == SINGLE_MODE)
	{
		wdata = 0x80;
	}
	else if (p_gp->operation_mode == CONTINUOUS_MODE)
	{
		wdata = 0xC0;
	}
	seq_i2c_write(slave_addr, 0x00, &wdata, 1);
}

void StopMeasurement(struct gp2ap01vt00f_data *p_gp)
{
	u8 wdata;

	wdata = 0x00;
	seq_i2c_write(slave_addr, 0x00, &wdata, 1);
}

void ReadRangeData(struct gp2ap01vt00f_data *p_gp)
{
	u8 rbuf_err_staus;
	u8 rbuf[27];

	/* error status */
	seq_i2c_read(slave_addr, 0x01, &rbuf_err_staus, 1);
	/* measured data */
	seq_i2c_read(slave_addr, 0x0e, rbuf, 27);

	p_gp->reg01h = rbuf_err_staus;
	p_gp->reg10h = rbuf[2] & 0xf0;
	p_gp->range_result  =
		(((u32) (rbuf[2] & 0x0F)) << 16) + (((u32) rbuf[1]) << 8) + (u32) rbuf[0];        /* RANGE VAL count */
	p_gp->ret_sig       =
		(((u32) rbuf[5]) << 16) + (((u32) rbuf[4]) << 8) + (u32) rbuf[3];            /* RETURN SIGNAL count */
	p_gp->ret_amb       =
		(((u32) rbuf[11]) << 16) + (((u32) rbuf[10]) << 8) + (u32) rbuf[9];            /* RETURN AMBIENT count */
	p_gp->range_result2 =
		(((u32) (rbuf[23] & 0x0F)) << 16) + (((u32) rbuf[22]) << 8) + (u32) rbuf[21];    /* RANGE2 VAL count */
	p_gp->ret_sig2      =
		(((u32) rbuf[26]) << 16) + (((u32) rbuf[25]) << 8) + (u32) rbuf[24];            /* RETURN2 SIGNAL count */
	return;
}

void GP2AP01VT_apply_xtalk_compensation(struct gp2ap01vt00f_data *p_gp)
{
	float SignalCoefficientM, SignalCoefficientS, CompensationRange;
	float EfficientSignal;

	EfficientSignal = (float) p_gp->ret_sig + 0.2 * (float) p_gp->ret_amb;

	SignalCoefficientM = EfficientSignal / p_gp->xTalkRetSig - 1;

	SignalCoefficientS = 4.434 * sin(p_gp->range_mm / 2248.16 * 2.0 * M_PI);

	// DEBUG_MSG_1F(SignalCoefficientM:, SignalCoefficientM);
	// DEBUG_MSG_1F(SignalCoefficientS:, SignalCoefficientS);

	CompensationRange = 150.0 * SignalCoefficientS / 2.0 / SignalCoefficientM + p_gp->range_mm;

//    DEBUG_MSG_1(before range_mm:, p_gp->range_mm);
	p_gp->range_mm = (u16) (CompensationRange + 0.5);
//	DEBUG_MSG_1(after range_mm:, p_gp->range_mm);
}

void CalculateDistance(struct gp2ap01vt00f_data *p_gp)
{
	float range_mm_cal;
	float range2_mm_cal;
	float signal0_amb1;
	float signal0_amb2;
	float soft_waf1;

	/* Calculate distance */
	range_mm_cal  = 0.00653154 * ((float) p_gp->range_result - p_gp->c1 * 256);
	range2_mm_cal = 0.00879246 * ((float) p_gp->range_result2 - p_gp->c2 * 256);

	/* Offset with panel */
	range_mm_cal  = range_mm_cal + p_gp->offset_with_panel;
	range2_mm_cal = range2_mm_cal + p_gp->offset_with_panel * 1.35;

	/* Signal check */
	signal0_amb1 = LOW_RET_SIG + (LOW_AMB_COEFF * p_gp->ret_amb) + p_gp->xTalkRetSig;
	signal0_amb2 = (LOW_AMB_COEFF * p_gp->ret_amb) + LOW_XTALK_COEFF * p_gp->xTalkRetSig;
	soft_waf1    = (500 * exp((-1) * range2_mm_cal / 300) + 20000 * exp((-1) * range2_mm_cal / 25)) * 0.87 +
				   p_gp->xTalkRetSig * 6.05 / 8.5;
	// printf("soft_waf1:%f\r\n", soft_waf1);

	if (((p_gp->reg01h & 0x10) == 0x10) ||
		/* internal circuit error2 */
		((p_gp->reg10h & 0x10) == 0x10) ||
		/* internal circuit error1 */
		((p_gp->reg01h & 0x04) == 0x04))
	{                                                    /* vcsel short */
		p_gp->range_mm     = 8888;
		p_gp->range_status = HARDWARE_ERROR;
	}
	else if ((p_gp->reg10h & 0x20) == 0x20)
	{                                                    /* warp aroud filter */
		p_gp->range_mm     = 8888;
		p_gp->range_status = WAF_ERROR;
	}
	else if (p_gp->ret_amb > 10800)
	{                                                                /* Too much ambient error */
		p_gp->range_mm     = 8888;
		p_gp->range_status = SIGNAL_ERROR;
	}
	else if ((p_gp->ret_sig > 60000) && ((range_mm_cal >= 2100) || (range_mm_cal < 0)))
	{        /* Close range error */
		p_gp->range_mm     = 0;
		p_gp->range_status = CLOSE_RANGE_ERROR;
	}
	else if (range_mm_cal < 0)
	{
		p_gp->range_mm     = 8888;
		p_gp->range_status = SIGNAL_ERROR;
	}
	else if ((p_gp->ret_sig < signal0_amb1) || (p_gp->ret_sig < signal0_amb2))
	{                /* Low return signal error */
		p_gp->range_mm     = 8888;
		p_gp->range_status = SIGNAL_ERROR;
	}
	else if ((p_gp->xTalkRetSig != 0) && ((p_gp->ret_sig2 < soft_waf1) ||                        /* Soft waf error */
										  ((p_gp->ret_sig2 < (330 + p_gp->xTalkRetSig * 6.06 / 8.5)) &&
										   (range2_mm_cal < 400))))
	{
		p_gp->range_mm     = 8888;
		p_gp->range_status = SOFT_WAF_ERROR;
	}
	else
	{                                                                                        /* Valid data */
		p_gp->range_mm     = (u16) range_mm_cal + 0.5;
		p_gp->range_status = VALID_DATA;
	}

	/* Crosstalk compensation */
	if ((p_gp->xTalkRetSig > 0) && (p_gp->range_mm != 8888))
		GP2AP01VT_apply_xtalk_compensation(p_gp);

	/* more than MAX */
	if ((p_gp->range_mm > MAX) && (p_gp->range_mm != 8888))
	{
		p_gp->range_mm     = MAX;
		p_gp->range_status = SIGNAL_ERROR;
	}

	/*  median */
	if (p_gp->num_median == 3)
		p_gp->num_median = 0;

	if ((p_gp->ret_sig_pre > (p_gp->ret_sig * 10)) ||
		(p_gp->ret_sig_pre < (p_gp->ret_sig / 10)))
	{
		p_gp->num_median = 0;
		p_gp->data_median_temp[0] = 0;
		p_gp->data_median_temp[1] = 0;
		p_gp->data_median_temp[2] = 8888;
	}

	p_gp->data_median_temp[p_gp->num_median] = p_gp->range_mm;

	// printf("%ld,%ld,%ld,%d,%d\r\n", p_gp->data_median_temp[0], p_gp->data_median_temp[1], p_gp->data_median_temp[2], p_gp->range_status, p_gp->num_median);

	p_gp->range_mm = func_median(p_gp->data_median_temp);

	if (((p_gp->range_status == 0) && ((p_gp->range_mm == 0) || (p_gp->range_mm == 8888))) ||
		((p_gp->range_status != 0) && ((p_gp->range_mm > 0) && (p_gp->range_mm < 8888))))
		p_gp->range_status = p_gp->range_status_pre;

	// printf("%ld,%d\r\n", p_gp->range_mm, p_gp->range_status);

	p_gp->num_median++;
	p_gp->range_status_pre = p_gp->range_status;
	p_gp->ret_sig_pre      = p_gp->ret_sig;

	return;
}

void GetRangingMeasurementData(struct gp2ap01vt00f_data *p_gp)
{
	/* Read range data via i2c */
	ReadRangeData(p_gp);

	/* Calculate distance */
	CalculateDistance(p_gp);
}

/* Set offset calibration data to calculation from NVM */
void SetOffsetCalibrationDataMilliMeter(struct gp2ap01vt00f_data *p_gp)
{
#ifdef PIC18F26K22
	long data;
    data = read_program_eeprom(0x200);
#endif
#ifdef ARDUINO_UNO
	s8 data;
    data = (signed int)EEPROM.read(0);
#endif

//	p_gp->offset_with_panel = (s8)data;
}

/* Set crosstalk calibration data to calculation from NVM */
void SetXTalkCompensationReturnSignal(struct gp2ap01vt00f_data *p_gp)
{
#ifdef PIC18F26K22
	long data;
    data = read_program_eeprom(0x300);
#endif
#ifdef ARDUINO_UNO
	int data;
    data = EEPROM.read(2);
#endif
//	p_gp->xTalkRetSig = (u16)data;
}

/* Check the offset calibration data */
void GetOffsetCalibrationDataMilliMeter(struct gp2ap01vt00f_data *p_gp)
{
#ifdef PIC18F26K22
	long data;
    data = read_program_eeprom(0x200);
#endif
#ifdef ARDUINO_UNO
	s8 data;
    data = (signed int)EEPROM.read(0);
#endif
//	DEBUG_MSG_0(* * * Offset with panel * * *);
//	DEBUG_MSG_1(offset_with_panel:, p_gp->offset_with_panel);
//	DEBUG_MSG_1(EEPROM offset_with_panel:, data);
}

/* Check the crosstalk calibration data */
void GetXTalkCompensationReturnSignal(struct gp2ap01vt00f_data *p_gp)
{
#ifdef PIC18F26K22
	long data;
    data = read_program_eeprom(0x300);
#endif
#ifdef ARDUINO_UNO
	int data;
    data = EEPROM.read(2);
#endif
//	DEBUG_MSG_0(* * * Crosstalk * * *);
//	DEBUG_MSG_1(xTalkRetSig:, p_gp->xTalkRetSig);
//	DEBUG_MSG_1(EEPROM xTalkRetSig:, data);
}


/* for calibration */
void GetRangingMeasurementDataAve(struct gp2ap01vt00f_data *p_gp)
{
	int i;

	StartMeasurement(p_gp);

	p_gp->range_mm_ave = 0;
	p_gp->ret_sig_ave  = 0;
	p_gp->ret_amb_ave  = 0;

	for (i = 0; i < p_gp->num_measure; i++)
	{
		//DELAY_FUNC(33)
		//Delayms(33); /* 33msec */

		GetRangingMeasurementData(p_gp);

		p_gp->range_mm_ave += p_gp->range_mm;
		p_gp->ret_sig_ave += p_gp->ret_sig;
		p_gp->ret_amb_ave += p_gp->ret_amb;
		// printf("ave:%4.2f, range_mm:%7Lu, range_status:%d\r\n", p_gp->range_mm_ave, p_gp->range_mm, p_gp->range_status);
//		DEBUG_MSG_1(range_mm:, p_gp->range_mm);
//		DEBUG_MSG_1(range_status:, (u32)p_gp->range_status);
	}

	p_gp->range_mm_ave = p_gp->range_mm_ave / p_gp->num_measure;
	p_gp->ret_sig_ave  = p_gp->ret_sig_ave / p_gp->num_measure;
	p_gp->ret_amb_ave  = p_gp->ret_amb_ave / p_gp->num_measure;

//	DEBUG_MSG_0(* * * Average of 10 times * * *);
//	DEBUG_MSG_1(range_mm_ave:, (u32)p_gp->range_mm_ave);
//	DEBUG_MSG_1(ret_sig_ave:, (u32)p_gp->ret_sig_ave);
//	DEBUG_MSG_1(ret_amb_ave:, (u32)p_gp->ret_amb_ave);

	StopMeasurement(p_gp);
}

void get_signal_judge_coefficientM(struct gp2ap01vt00f_data *p_gp, float SignalCoefficientS, float *SignalCoefficientM)
{
	int         Status         = 0;
	const float vcsel_pulse    = 6.41;
	const float spad_pulse     = 3.7;
	// const float meas_pulse = 15.0;
	float       diffRangeNs    = (float) (p_gp->xTalkCalDistanceAsInt - p_gp->range_mm_ave) / 150.0;
	float       paramSoverM[2] = {0.0};
	float       bound[2]       = {
		0.8,
		4.37
	};

	paramSoverM[0] = 2.0 * diffRangeNs;
	paramSoverM[1] = vcsel_pulse
					 - pow((vcsel_pulse + spad_pulse) / 2.0 - diffRangeNs, 2.0) / spad_pulse;

	if (0.0 <= paramSoverM[0] && paramSoverM[0] < bound[0])
	{
		*SignalCoefficientM = SignalCoefficientS / paramSoverM[0];
	}
	else if (bound[0] <= paramSoverM[1] && paramSoverM[1] < bound[1])
	{
		*SignalCoefficientM = SignalCoefficientS / paramSoverM[1];
	}
	else
	{
//		DEBUG_MSG_0(xtalk calibration error!!!);
	}

	if (Status == 1)
	{
		if (*SignalCoefficientM <= 1.0)
			Status = -1;
	}
}

void GP2AP01VT_calc_xtalk_signal(struct gp2ap01vt00f_data *p_gp)
{
	float SignalCoefficientS, SignalCoefficientM;
	float EfficientSignal_ave;

	EfficientSignal_ave = p_gp->ret_sig_ave + 0.2 * p_gp->ret_amb_ave;
	SignalCoefficientS  = 4.434 * sin(p_gp->range_mm_ave / 2248.16 * 2.0 * M_PI);

	get_signal_judge_coefficientM(p_gp, SignalCoefficientS, &SignalCoefficientM);

	p_gp->xTalkRetSig = (8.507 / 19.516) * (EfficientSignal_ave / (1.0 + SignalCoefficientM) + 0.5);

//	DEBUG_MSG_1(xTalkRetSig:, (u32)p_gp->xTalkRetSig);
	// DEBUG_MSG_1F(S:, SignalCoefficientS);
	// DEBUG_MSG_1F(M:, SignalCoefficientM);
}

/* Gray Card 17%, distance:600mm, dark condition at factory
   xTalkRetSig(counts) : Crosstalk value from the panel
   Please optical design so that crosstalk value is less than 200 counts */
void PerformXTalkCalibration(struct gp2ap01vt00f_data *p_gp)
{
	u8 wdata;

	p_gp->xTalkRetSig = 0;

	/* 15nsec mode, roa:19.5msec */
	wdata = 0x4F;
	seq_i2c_write(slave_addr, 0x06, &wdata, 1);
	wdata = 0x25;
	seq_i2c_write(slave_addr, 0x0D, &wdata, 1);

	/* Perform 10 measurements and compute the averages */
	p_gp->num_measure = 10;
	GetRangingMeasurementDataAve(p_gp);

	//p_gp->range_mm_ave = 500;
	//p_gp->ret_sig_ave = 4000;
	//p_gp->ret_amb_ave =  288;
	// xTalkRetSig is approximately 800(19.5),  350(8.5msec)
	if ((p_gp->xTalkCalDistanceAsInt == 0) ||
		(p_gp->range_mm_ave >= p_gp->xTalkCalDistanceAsInt))
	{
		p_gp->xTalkRetSig = 0;
//		DEBUG_MSG_0(xtalk_calibration error1);
		return;
	}
	else
	{
		GP2AP01VT_calc_xtalk_signal(p_gp);
	}

	if (p_gp->xTalkRetSig <= 285)
	{
//		DEBUG_MSG_0(xtalk_calibration success);
	}
	else
	{
//		DEBUG_MSG_0(xtalk_calibration error2);
	}

	/* Release Test setting */
	wdata = 0x0F;
	seq_i2c_write(slave_addr, 0x06, &wdata, 1);
	wdata = 0x34;
	seq_i2c_write(slave_addr, 0x0D, &wdata, 1);
}

/* White card 88%, distance:100mm, dark condition at factory */
void PerformOffsetCalibration(struct gp2ap01vt00f_data *p_gp)
{
	s16 offset;

	p_gp->offset_with_panel = 0;

	p_gp->num_measure = 10;
	GetRangingMeasurementDataAve(p_gp);

	/* distance:100mm */
	offset = p_gp->OffsetCalDistanceAsInt - p_gp->range_mm_ave + 0.5;

	if ((offset < 100) && (offset > -100))
	{
		p_gp->offset_with_panel = offset;
//		DEBUG_MSG_1(Offset_calibration success:, p_gp->offset_with_panel);
	}
	else
	{
		p_gp->offset_with_panel = 0;
//		DEBUG_MSG_0(Offset_calibration error);
	}
}

int GetMeasurementDataReady(struct gp2ap01vt00f_data *p_gp)
{
	int status;
	u8  rbuf;

	seq_i2c_read(slave_addr, 0x01, &rbuf, 1);

	if ((rbuf & 0x02) == 0x02)
		status = true;
	else
		status = false;

	return status;
}

void ClearInterruptMask(struct gp2ap01vt00f_data *p_gp)
{
	u8 wdata;

	wdata = 0xFD;
	seq_i2c_write(slave_addr, 0x01, &wdata, 1);
}

void GetTrimmingData(struct gp2ap01vt00f_data *p_gp)
{
	u8 rbuf;
	u8 reg50h, reg51h, reg52h, reg53h;

	seq_i2c_read(slave_addr, 0x50, &rbuf, 1);
	reg50h = rbuf;
	seq_i2c_read(slave_addr, 0x51, &rbuf, 1);
	reg51h = rbuf;
	seq_i2c_read(slave_addr, 0x52, &rbuf, 1);
	reg52h = rbuf;
	seq_i2c_read(slave_addr, 0x53, &rbuf, 1);
	reg53h = rbuf;

//	DEBUG_MSG_0(* * * Trimming data * * *);
//	DEBUG_MSG_1HEX(0x50:, reg50h);
//	DEBUG_MSG_1HEX(0x51:, reg51h);
//	DEBUG_MSG_1HEX(0x52:, reg52h);
//	DEBUG_MSG_1HEX(0x53:, reg53h);

	return;
}

void GetVersion()
{
#ifdef PIC18F26K22
	printf("Software ver:%s\r\n", SOFT_REVISION);
#endif
#ifdef ARDUINO_UNO
	Serial.print("Software ver:");
    Serial.println(SOFT_REVISION);
#endif
}

void GetDeviceInfo()
{
	u8 rbuf;

	seq_i2c_read(slave_addr, 0x3B, &rbuf, 1);
//	DEBUG_MSG_1HEX(Device code:, rbuf);
}

