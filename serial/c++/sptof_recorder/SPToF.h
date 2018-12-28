#ifndef _SPTOF_H_
#define _SPTOF_H_

#include <sys/poll.h>
#include "gp2ap01vt00f_api.h"
#include "Locker.h"

#define CHIP "/dev/psensor"
#define PSENSOR_IOCTL_MAGIC 'p'
#define PSENSOR_IOCTL_GET_ENABLED       _IOR(PSENSOR_IOCTL_MAGIC, 1, int *)
#define PSENSOR_IOCTL_ENABLE            _IOW(PSENSOR_IOCTL_MAGIC, 2, int *)
#define PSENSOR_IOCTL_DISABLE           _IOW(PSENSOR_IOCTL_MAGIC, 3, int *)
#define PSENSOR_IOCTL_RW_REG            _IOWR(PSENSOR_IOCTL_MAGIC, 4, int *)

#define PSENSOR_NAME_LENGTH             128

typedef struct
{
    char addr;
    char value;
    char rw;
} PsensorRWReg;


typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int rx;
    unsigned int distance;
} PsensorAxis;


typedef struct
{
    int gsfd;
    int gsinput;
    int gsthread_state;
    int gstate;
    int gsrelease_state;
    int axis_x_init;
    int axis_y_init;
    int axis_z_init;
    int axis_rx_init;
    int axis_distance_init;
    int gsfun_count;

    struct pollfd fds;
    PsensorAxis   gsdata;
} Psensor;


class SPToF
{
public:
    /**
     *  @fn	    SPToF()
     *	@brief	constructor function
     */
    SPToF();

    /**
     *  @fn	    ~SPToF()
     *	@brief	destructor function
     */
    ~SPToF();

    int Init(int iOffset, int iXTalk);

    void DeInit();

    unsigned short GetSPToFDistance();

private:
    pthread_t m_ThreadID;

    // The condition variable for read data
    cond m_ReadDataCond;

    bool m_bReadNewToFData;

    unsigned short m_usDistance;

    Psensor m_Psensor;

    struct gp2ap01vt00f_data m_gp;

    int m_offset = 0;

    int m_xtalk = 0;

    bool m_bStop;

    int EnablePsensor(int enable);

    int OpenInput(const char *inputname);

    void SPToFRoutine();

    static void *SPToFThread(void *arg);

    int RWReg(PsensorRWReg *reg, u32 *value);

    int GetC1C2();

    int PsensorResetSensor();

    int Readevent(PsensorAxis *absdata);
};


#endif //_SPTOF_H_
