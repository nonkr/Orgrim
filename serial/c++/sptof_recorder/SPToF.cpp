#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <exception>
#include "SPToF.h"
//#include "Tools/PrintUtil.h"
//#include "Tools/TimeUtil.h"

PsensorRWReg g_psensor_rw_cmd[] =
                 {
                     {0x52, 0,    1},
                     {0x53, 0,    1},
                     {0x06, 0x4F, 0},
                     {0x0D, 0x25, 0},
                     {0x06, 0x0F, 0},
                     {0x0D, 0x34, 0},
                     {0x06, 0,    1},
                     {0x0D, 0,    1},
                 };

PsensorRWReg psensor_reset_cmd[] =
                 {
                     {0,    0,    0},
                     {2,    1,    0},
                     {2,    0x12, 0},
                     {3,    0,    0},
                     //{4,0xef},for Avdd=2.8V
                     {4,    0x6f, 0},//for Avdd=3.3V duty=30% if=59ma
                     {5,    0x1f, 0},
                     //{6,0x0f},//for Avdd=2.8V
                     {6,    0x09, 0},//for Avdd=3.3V
                     {7,    0xb3, 0},
                     {8,    0,    0},
                     {0x0a, 0x11, 0},
                     {0x0b, 0x06, 0},
                     {0x0d, 0x34, 0},
                     {0x46, 0x74, 0},
                     {0x47, 0x5f, 0},
                     {0x00, 0xc0, 0},//auto continue mode

                 };


/**
 *  @fn	    SPToF()
 *	@brief	constructor function
 */
SPToF::SPToF()
{
    memset(&m_Psensor, 0, sizeof(m_Psensor));
    m_Psensor.gsfd    = -1;
    m_Psensor.gsinput = -1;
    m_bStop = false;
}

/**
 *  @fn	    ~SPToF()
 *	@brief	destructor function
 */
SPToF::~SPToF()
{
}

int SPToF::Init(int iOffset, int iXTalk)
{
    m_Psensor.gsfd = open(CHIP, O_RDWR);
    if (m_Psensor.gsfd < 0)
    {
        printf("failed to open Psensor dev");
        return 1;
    }

    m_Psensor.gsinput = OpenInput("proximity");
    if (m_Psensor.gsinput < 0)
    {
        printf("failed to open Psensor input dev");
        DeInit();
        return 1;
    }
    m_Psensor.fds.fd     = m_Psensor.gsinput;
    m_Psensor.fds.events = POLLIN;

    m_offset = iOffset;
    m_xtalk  = iXTalk;

    if (pthread_create(&m_ThreadID, nullptr, SPToFThread, this))
    {
        printf("Failed to create SPToFThread\n");
        DeInit();
        return 1;
    }

    return 0;
}

void SPToF::DeInit()
{
    m_bStop = true;
    pthread_join(m_ThreadID, nullptr);

    if (m_Psensor.gsfd >= 0)
    {
        close(m_Psensor.gsfd);
        m_Psensor.gsfd = -1;
    }

    if (m_Psensor.gsinput >= 0)
    {
        close(m_Psensor.gsinput);
        m_Psensor.gsinput = -1;
    }
}

int SPToF::OpenInput(const char *inputname)
{
    int           fd           = -1;
    const char    *dirname     = "/dev/input";
    char          devname[512] = {0};
    char          *filename;
    DIR           *dir;
    struct dirent *de;

    dir = opendir(dirname);
    if (dir == nullptr)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while ((de = readdir(dir)))
    {
        if (de->d_name[0] == '.' &&
            (de->d_name[1] == '\0' ||
             (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        fd = open(devname, O_RDONLY);
        if (fd >= 0)
        {
            char name[PSENSOR_NAME_LENGTH] = {0};

            if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1)
                name[0] = '\0';

            if (!strcmp(name, inputname))
            {
                break;
            }
            else
            {
                close(fd);
                fd = -1;
            }
        }
    }
    closedir(dir);

    return fd;
}

int SPToF::EnablePsensor(int enable)
{
    int ret;

    if (enable)
    {
        ret = ioctl(m_Psensor.gsfd, PSENSOR_IOCTL_ENABLE, &enable);
        if (ret < 0)
        {
            printf("Failed to enable Psensor\n");
            return -1;
        }
        m_Psensor.gstate = 1;
    }
    else
    {
        ret = ioctl(m_Psensor.gsfd, PSENSOR_IOCTL_ENABLE, &enable);
        if (ret < 0)
        {
            printf("Failed to disable Psensor\n");
            return -1;
        }
        m_Psensor.gstate = 0;
    }

    return ret;
}

void SPToF::SPToFRoutine()
{
    PsensorAxis new_gs_data;

    // init
    DataInit(&m_gp);

    m_gp.offset_with_panel = m_offset;
    m_gp.xTalkRetSig       = m_xtalk;

    // enable Psensor
    if (EnablePsensor(1) < 0)
    {
        return;
    }

    // get c1 and c2
    if (GetC1C2() < 0)
    {
        return;
    }

    // read m_Psensor data in loops
    memset(&new_gs_data, 0, sizeof(new_gs_data));
    while (!m_bStop)
    {
        //get the rang value saved in m_gp struct
        if (!Readevent(&new_gs_data))
        {
            m_ReadDataCond.lock();

            m_usDistance      = m_gp.range_mm;
            m_bReadNewToFData = true;

            m_ReadDataCond.signal();
            m_ReadDataCond.unlock();
        }
    }
}

void *SPToF::SPToFThread(void *arg)
{
    SPToF *pSPToF = (SPToF *) arg;

    prctl(PR_SET_NAME, "SPT");

    pSPToF->SPToFRoutine();


    pthread_exit(nullptr);
}

/*read hardware register directory by ioctl*/
int SPToF::RWReg(PsensorRWReg *reg, u32 *value)
{
    int ret;
    int __data__;

    __data__ = reg->addr + ((reg->value) << 8) + ((reg->rw) << 16);
    ret      = ioctl(m_Psensor.gsfd, PSENSOR_IOCTL_RW_REG, &__data__);
    if (ret < 0)
    {
        return -1;
    }

    if (reg->rw)//read
        *value = (u32) __data__;
    return ret;
}

int SPToF::GetC1C2()
{
    PsensorRWReg *rwop = &g_psensor_rw_cmd[0];
    if (RWReg(rwop, &m_gp.c1) < 0)
    {
        printf("Failed to register Psensor\n");
        return 1;
    }
    rwop = &g_psensor_rw_cmd[1];
    if (RWReg(rwop, &m_gp.c2) < 0)
    {
        printf("Failed to register Psensor\n");
        return 1;
    }

    return 0;
}

int SPToF::PsensorResetSensor()
{
    int ret;

    for (int i = 0; i < (int) (sizeof(psensor_reset_cmd) / sizeof(PsensorRWReg)); i++)
    {
        ret = RWReg(&psensor_reset_cmd[i], 0);
        if (psensor_reset_cmd[i].value == 1 && psensor_reset_cmd[i].addr == 2)
        {
            usleep(1000);
        }
        if (ret < 0)
        {
            printf("reset single sensor failed\n");
            break;
        }
    }
    return ret;
}

int SPToF::Readevent(PsensorAxis *absdata)
{
    struct input_event event;
    int                ret             = 0;
    int                iPollErrorCount = 0;

    while (1)
    {
        ret = poll(&m_Psensor.fds, 1, 1000);
        if (ret == -1)
        {
            perror("MSPToF poll()");
            iPollErrorCount++;
        }
        else if (ret == 0)
        {
            printf("MSPToF poll time out, going to reset psensor\n");
            PsensorResetSensor();
        }
        else
        {
            if (read(m_Psensor.gsinput, &event, sizeof(event)) < 0)
            {
                return 1;
            }

            if (event.type == EV_ABS)
            {
                if (event.code == ABS_X)
                {
                    absdata->x = event.value;
                }
                else if (event.code == ABS_Y)
                {
                    absdata->y = event.value;
                }
                else if (event.code == ABS_Z)
                {
                    absdata->z = event.value;
                }
                else if (event.code == ABS_RX)
                {
                    absdata->rx = event.value;
                }
                else if (event.code == ABS_DISTANCE)
                {
                    absdata->distance = event.value;
                }
            }
            else if (event.type == EV_SYN)
            {
                m_gp.reg01h        = ((absdata->rx >> 24) & 0xff);      // rbuf_err_staus;
                m_gp.reg10h        = (absdata->distance >> 16) & 0xf0;  // rbuf[2] & 0xf0;
                m_gp.range_result  = (absdata->distance & 0xfffff);     // RANGE VAL count
                m_gp.ret_sig       = (absdata->rx & 0xffffff);          // RETURN SIGNAL count
                m_gp.ret_amb       = absdata->z;                        // RETURN AMBIENT count
                m_gp.range_result2 = absdata->y;                        // RANGE2 VAL count
                m_gp.ret_sig2      = absdata->x;                        // RETURN2 SIGNAL count
                CalculateDistance(&m_gp);
                break;
            }
            else
            {
                printf("Psensor unknown event");
                return 1;
            }
        }
    }

    return 0;
}

unsigned short SPToF::GetSPToFDistance()
{
    unsigned short usDistance;

    m_ReadDataCond.lock();
    m_bReadNewToFData = false;
    while (!m_bReadNewToFData)
    {
        m_ReadDataCond.wait();
    }

    usDistance = m_usDistance;

    m_ReadDataCond.unlock();

    return usDistance;
}
