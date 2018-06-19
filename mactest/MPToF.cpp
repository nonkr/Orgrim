/*
 * Copyright: Hikvision Software Co.Ltd. All Rights Reserved.
 * File: MsgMgr.h
 * Description: Manage the message through over each module.
 * Author: songbinbin
 * Date: 2018/02/11
 * Changelog:
 *   v1.0 2018/02/11 first revision
 */

#include <fstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/types.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <sys/prctl.h>
#include <exception>
#include <chrono>
#include <cmath>
#include <sys/stat.h>
#include <sys/mman.h>

#include "ionbuffer.h"
#include "MPToF.h"
#include "TFCCtrl.h"
#include "../color.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define AUDIO_REG_BASE   (0x10300000)
#define MAP_SIZE         (0xFF)
#define REG_BOOTFLAG     (0x580)
#define REG_BOOTMODE     (0x584)
#define REG_BOOTCNT0     (0x588)
#define REG_BOOTCNT1     (0x58c)

using namespace std;
using namespace chrono;

/**
 *  @fn	    MPToF()
 *	@brief	constructor function
 */
MPToF::MPToF()
{
}

/**
 *  @fn	    ~MPToF()
 *	@brief	destructor function
 */
MPToF::~MPToF()
{
}

int ResetUpdateFlag()
{
    int nDevFd = open("/dev/mem", O_RDWR | O_NDELAY);
    if (nDevFd < 0)
    {
        printf("open(/dev/mem) failure!\n");
        return 1;
    }
    unsigned char *MapBase = (unsigned char *) mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, nDevFd,
                                                    AUDIO_REG_BASE);
    *(volatile unsigned int *) (MapBase + REG_BOOTCNT1) = 0;

    if (nDevFd)
    {
        close(nDevFd);
    }
    munmap(MapBase, MAP_SIZE);

    return 0;
}

int MPToF::Init()
{
    ResetUpdateFlag();

    // new tof context
    try
    {
        m_pVideoContext = new VideoContext;
        memset(m_pVideoContext, 0, sizeof(VideoContext));
    }
    catch (...)
    {
        goto error;
    }

    // open tof device
    if ((m_pVideoContext->fd = OpenCameraDevice()) < 0)
    {
        goto error;
    }

    // init tof device
    if (InitCameraDevice(m_pVideoContext))
    {
        goto error;
    }

    if (MPToF::AllocIonMemory(m_pVideoContext))
    {
        goto error;
    }

    if (StartCapture(m_pVideoContext))
    {
        goto error;
    }

    if (CheckToFPattern())
    {
        OGM_PRINT_GREEN("CheckPattern: ok\n");
    }
    else
    {
        OGM_PRINT_RED("CheckPattern: detect vertical bar, start rebooting now\n");
        system("/sbin/reboot");
    }

    return 0;
error:
    DeInit();
    return 1;
}

void MPToF::DeInit()
{
    if (m_pVideoContext)
    {
        StopCapture(m_pVideoContext);
        CloseCameraDevice(m_pVideoContext);
        delete m_pVideoContext;
        m_pVideoContext = nullptr;
    }
}

static int WriteBinaryData(char *data, int size, const char *type)
{
    char fname[50] = {0};
    snprintf(fname, 30, "/tmp/algo/%s.bin", type);
    FILE *yuv_file = fopen(fname, "w+");
    if (yuv_file)
    {
        fwrite(data, size, 1, yuv_file);
        fclose(yuv_file);
    }
    else
        printf("open file %s error", fname);

    return SS_SUCCESS;
}

int MPToF::CheckToFPattern()
{
    int            iRet         = 0;
    char           arrToFDataBuffer[TOFRAWDATASIZE];
    unsigned short usAmplitude[MPTOF_PIXELS];
    int            i, j, k;
    int            iToFDataSize = TOFDATASIZE;

    setMacTestMode(m_pVideoContext->fd);

    do
    {
        iRet = ReadFrame(m_pVideoContext, arrToFDataBuffer);
    } while (iRet != 0);

    for (i = 0, k = 0; i < iToFDataSize; i += 32)
    {
        for (j = 0; j < 16; j += 2)
        {
            usAmplitude[k] = (unsigned short) ((arrToFDataBuffer[(i + j) * 2]) |
                                               (arrToFDataBuffer[(i + j + 1) * 2] << 8 & 0x0F00));
            k++;
        }
    }

//    WriteBinaryData((char *) usAmplitude, sizeof(usAmplitude), "Amplitude");

    iRet = checkPattern(usAmplitude, sizeof(usAmplitude));
    printf("checkPattern return %d\n", iRet);
    if (iRet == -1)
    {
        printf("checkPattern Invalid input parameter\n");
        clrMacTestMode(m_pVideoContext->fd);
        return 1;
    }
    else if (iRet == 1)
    {
        // normal
        clrMacTestMode(m_pVideoContext->fd);
        return 1;
    }

    clrMacTestMode(m_pVideoContext->fd);

    // abnormal with vertical bar
    return 0;
}


int MPToF::SetCtrl(int fd, unsigned int id, unsigned int value)
{
    struct v4l2_control ctrl;
    int                 ret;

    ctrl.id    = id;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);

    if (ret < 0)
    {
        fprintf(stderr, "Failed to ioctl VIDIOC_S_CTRL id:[0x%02X] value:[0x%02X]\n", id, value);
    }

    return ret;
}

int MPToF::OpenCameraDevice()
{
    int fd = open(HKTOF_DEVICE, O_RDWR);
    if (fd < 0)
    {
        printf("Failed to open %s\n", HKTOF_DEVICE);
        return -SS_ERR_OPEN_CAMERA;
    }

    return fd;
}

int MPToF::AllocIonMemory(VideoContext *vc)
{
    int i;

    for (i = 0; i < CACHE_BUFFER_NUM; i++)
    {
        vc->yuv[i] = AllocIonBuffer(2 * MPTOF_WIDTH, 2 * MPTOF_HEIGHT);
        if (!vc->yuv[i])
        {
            printf("alloc ion buffer return failed\n");
            return SS_ERR_ION;
        }
    }

    struct v4l2_requestbuffers reqbufs; // 申请视频缓冲区
    memset(&reqbufs, 0, sizeof(reqbufs));
    reqbufs.count  = 4;
    reqbufs.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbufs.memory = V4L2_MEMORY_USERPTR;

    if (-1 == ioctl(vc->fd, VIDIOC_REQBUFS, &reqbufs))
    {
        printf("Fail to ioctl 'VIDIOC_REQBUFS'\n");
        return SS_ERR_VIDIOC_REQBUFS;
    }

    return SS_SUCCESS;
}

int MPToF::Init9221Config(VideoContext *vc)
{
    // Setting IND_FREQ_DATA_SEL = 1
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x29304048) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // Follow segment setting F2_QX_COEFF
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x10000000) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x110079bb) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x12004bcc) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x1300b4c4) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x14008645) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x15000000) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x16000000) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x1700278e) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x18009873) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x19009873) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x1a00278e) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x1b007fff) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // PIX_CNT_MAX = 200000 -> 400000
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG58, 0x82061a80) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // SUB_FRAME=2 QUAD_CNT=6
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG58, 0x83000016) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // DEBUG_QUAD_NUMBER=2
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG58, 0x0f000c9a) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // MOD_F1
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG58, 0x0c280000) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // set IQ_SCALE increase phase digital resolution
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x1f254321) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // Setting HDR_CAL=4 And int_duty_cycle=12
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG58, 0x0200001c) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG58, 0x02000018) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x4c80000c) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    // mactest
//    if (SetCtrl(vc->fd, HKTOF_CMD_SETLIGHT, 0x31001803) < 0)
//    {
//        return SS_ERR_VIDIOC_S_CTRL;
//    }

    // Setting Laser currency power pwm duty cycle
    if (SetCtrl(vc->fd, HKTOF_CMD_SETLIGHT, 0x00000032) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    return SS_SUCCESS;
}

int MPToF::InitCameraDevice(VideoContext *vc)
{
    struct v4l2_capability cap;
    struct v4l2_format     stream_fmt;
    struct v4l2_fmtdesc    fmt;
    struct v4l2_streamparm Stream_Parm;
    struct v4l2_input      input;
    int                    ret;

    memset(&fmt, 0, sizeof(fmt));
    fmt.index = 0;
    fmt.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (SetCtrl(vc->fd, HKTOF_CMD_INIT, 0) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    /*
     * 默认值为：0x240ffc44。修改为0x240ffc43帮助消除竖线
     */
    if (SetCtrl(vc->fd, HKTOF_CMD_SETREG5C, 0x240ffc43) < 0)
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    if (Init9221Config(vc))
    {
        return SS_ERR_VIDIOC_S_CTRL;
    }

    ret = ioctl(vc->fd, VIDIOC_QUERYCAP, &cap); //查询视频设备驱动的功能
    if (ret < 0)
    {
        printf("Fail to ioctl VIDEO_QUERYCAP\n");
        // TODO: write to log
        return SS_ERR_VIDIOC_QUERYCAP;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        printf("The Current device does not support streaming i/o\n");
        // TODO: write to log
        return SS_ERR_V4L2_CAP;
    }

    input.index = 0;
    ioctl(vc->fd, VIDIOC_ENUMINPUT, &input);
    printf("Name of input channel[%d] is %s\n", input.index, input.name);

    ret = ioctl(vc->fd, VIDIOC_S_INPUT, &input);
    if (ret < 0)
    {
        printf("Fail to set input\n");
        // TODO: write to log
        return SS_ERR_VIDIOC_S_INPUT;
    }

    memset(&stream_fmt, 0, sizeof(stream_fmt));
    stream_fmt.type                 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    stream_fmt.fmt.pix.width        = MPTOF_WIDTH;
    stream_fmt.fmt.pix.height       = MPTOF_HEIGHT;
    stream_fmt.fmt.pix.pixelformat  = V4L2_PIX_FMT_SGBRG8;
    stream_fmt.fmt.pix.sizeimage    = stream_fmt.fmt.pix.width * stream_fmt.fmt.pix.height * 8 / 8;
    stream_fmt.fmt.pix.bytesperline = stream_fmt.fmt.pix.width;

    ret = ioctl(vc->fd, VIDIOC_S_FMT, &stream_fmt);
    if (ret < 0)
    {
        printf("Fail to ioctl 'VIDIOC_S_FMT'\n");
        // TODO: write to log
        return SS_ERR_VIDIOC_S_FMT;
    }

    memset(&Stream_Parm, 0, sizeof(struct v4l2_streamparm));
    Stream_Parm.type                                  = V4L2_BUF_TYPE_VIDEO_CAPTURE; //要注意的是在这里，不管是overaly还是capture，这里的buffer type都是选择的V4L2_BUF_TYPE_VIDEO_CAPTURE
    Stream_Parm.parm.capture.timeperframe.numerator   = 1;
    Stream_Parm.parm.capture.timeperframe.denominator = MPTOF_FPS;

    ret = ioctl(vc->fd, VIDIOC_S_PARM, &Stream_Parm);
    if (ret < 0)
    {
        printf("VIDIOC_S_PARM failed!\n");
        // TODO: write to log
        return SS_ERR_VIDIOC_S_PARM;
    }

    return SS_SUCCESS;
}

int MPToF::StartCapture(VideoContext *vc)
{
    static unsigned int i = 0;
    enum v4l2_buf_type  type;

    for (i = 0; i < CACHE_BUFFER_NUM; i++)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory    = V4L2_MEMORY_USERPTR;
        buf.index     = i;
        buf.m.userptr = (unsigned long) vc->yuv[i]->vaddr;
        buf.length    = vc->yuv[i]->mem_size;

        if (-1 == ioctl(vc->fd, VIDIOC_QBUF, &buf))
        {
            printf("failed to ioctl 'VIDIOC_QBUF'\n");
            // TODO: write to log
            return SS_ERR_VIDIOC_QBUF;
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(vc->fd, VIDIOC_STREAMON, &type))
    {
        printf("failed to ioctl VIDIOC_STREAMON\n");
        close(vc->fd);
        return SS_ERR_VIDIOC_STREAMON;
    }

    return 0;
}

int MPToF::ReadFrame(VideoContext *vc, char *buffer)
{
    struct v4l2_buffer buf;
    unsigned int       i;
    int                ret;
    int                retrycount = 3;
    struct pollfd      pfd;

    pfd.fd     = vc->fd;
    pfd.events = POLLIN | POLLERR;

    while (retrycount > 0)
    {
        ret = poll(&pfd, 1, 1000);
        if (ret < 0)
        {
            perror("polling error");
            return ret;
        }
        else if (!ret)
        {
            printf("MPToF ReadFrame timedout in 1 sec.\n");
            // TODO: write to log
            return ETIMEDOUT;
        }

        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        ret = ioctl(vc->fd, VIDIOC_DQBUF, &buf);
        if (ret < 0)
        {
            retrycount--;
            perror("DQBUF failed");
        }
        else
            break;
    }

    for (i = 0; i < CACHE_BUFFER_NUM; ++i)
    {
        if (buf.m.userptr == (unsigned long) vc->yuv[i]->vaddr)
            break;
    }

    memcpy(buffer, vc->yuv[i]->vaddr, TOFRAWDATASIZE);

    if (-1 == ioctl(vc->fd, VIDIOC_QBUF, &buf))
    {
        printf("Fail to ioctl 'VIDIOC_QBUF'\n");
        return SS_ERR_VIDIOC_QBUF;
    }

    return SS_SUCCESS;
}

void MPToF::StopCapture(VideoContext *vc)
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(vc->fd, VIDIOC_STREAMOFF, &type))
    {
        printf("Fail to ioctl 'VIDIOC_STREAMOFF'\n");
        // TODO: write to log
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(vc->ispfd, VIDIOC_STREAMOFF, &type);

    for (unsigned int i = 0; i < CACHE_BUFFER_NUM; i++)
    {
        if (vc->yuv[i])
        {
            vc->yuv[i]->free(vc->yuv[i]);
        }
    }
    return;
}

void MPToF::CloseCameraDevice(VideoContext *vc)
{
    close(vc->fd);
    return;
}
