/*
 * Copyright: Hikvision Software Co.Ltd. All Rights Reserved.
 * File: MsgMgr.h
 * Description: Manage the message through over each module.
 * Author: songbinbin
 * Date: 2018/02/11
 * Changelog:
 *   v1.0 2018/02/11 first revision
 */

#ifndef _MPTOF_H_
#define _MPTOF_H_

#include <pthread.h>
#include <linux/videodev2.h>

#define MPTOF_WIDTH  (1280)
#define MPTOF_HEIGHT (240)
#define MPTOF_FPS    (20)
#define CACHE_BUFFER_NUM (4)
#define MPTOF_PIXELS (320*240)
#define TOFDATASIZE (MPTOF_PIXELS*4)
#define TOFRAWDATASIZE (TOFDATASIZE*2)

#define HKTOF_DEVICE "/dev/video5"

#define HKTOF_CMD_INIT (V4L2_CID_PRIVATE_BASE+1)
#define HKTOF_CMD_SETREG58 (V4L2_CID_PRIVATE_BASE+2)
#define HKTOF_CMD_SETREG5C (V4L2_CID_PRIVATE_BASE+3)
#define HKTOF_CMD_PRINTALLREG (V4L2_CID_PRIVATE_BASE+4)
#define HKTOF_CMD_SETTESTMODE (V4L2_CID_PRIVATE_BASE+5)
#define HKTOF_CMD_SETLIGHT (V4L2_CID_PRIVATE_BASE+6)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
                      (byte & 0x80 ? '1' : '0'), \
                      (byte & 0x40 ? '1' : '0'), \
                      (byte & 0x20 ? '1' : '0'), \
                      (byte & 0x10 ? '1' : '0'), \
                      (byte & 0x08 ? '1' : '0'), \
                      (byte & 0x04 ? '1' : '0'), \
                      (byte & 0x02 ? '1' : '0'), \
                      (byte & 0x01 ? '1' : '0')

#define HDR_FLAG_BIT_POS (6)
#define FRAME_IS_NORMAL(byte) ((byte) & (0x01 << HDR_FLAG_BIT_POS)) > 1


/**
 * The definition of return code
 */
typedef enum
{
    SS_SUCCESS = 0,
    SS_ERR_MALLOC,
    SS_ERR_INIT_THREAD_MUTEX,
    SS_ERR_INIT_THREAD_COND,
    SS_ERR_INIT_THREAD,
    SS_ERR_INIT_CAMERA_DEVICE,
    SS_ERR_START_CAPTURE,
    SS_ERR_OPEN_CAMERA,
    SS_ERR_OPEN_PSENSOR,
    SS_ERR_OPEN_INPUT,
    SS_ERR_ION,
    SS_ERR_VIDIOC_S_CTRL,
    SS_ERR_VIDIOC_REQBUFS,
    SS_ERR_VIDIOC_QUERYCAP,
    SS_ERR_VIDIOC_S_INPUT,
    SS_ERR_VIDIOC_S_FMT,
    SS_ERR_VIDIOC_S_PARM,
    SS_ERR_VIDIOC_QBUF,
    SS_ERR_VIDIOC_STREAMON,
    SS_ERR_V4L2_CAP,
    SS_ERR_PSENSOR_IOCTL_ENABLE,
    SS_ERR_READ_PSENSOR,
    SS_ERR_UNKNOWN_PSENSOR_EVENT,
    SS_ERR_TIMEDOUT,
    SS_ERR_MPTOF_NOT_AVAILABLE,
} SS_RET;


struct VideoContext
{
    int              fd, subfd, ispfd;
    struct IonBuffer *yuv[CACHE_BUFFER_NUM];
    int              nframes;
};

class MPToF
{
public:
    /**
     *  @fn	    MPToF()
     *	@brief	constructor function
     */
    MPToF();

    /**
     *  @fn	    ~MPToF()
     *	@brief	destructor function
     */
    ~MPToF();

    int Init();

    void DeInit();

private:
    VideoContext *m_pVideoContext;

    int SetCtrl(int fd, unsigned int id, unsigned int value);

    int OpenCameraDevice();

    int InitCameraDevice(VideoContext *vc);

    int Init9221Config(VideoContext *vc);

    int StartCapture(VideoContext *vc);

    void StopCapture(VideoContext *vc);

    void CloseCameraDevice(VideoContext *vc);

    static int AllocIonMemory(VideoContext *vc);

    int CheckToFPattern();

    int ReadFrame(VideoContext *vc, char *buffer);
};

#endif //_MPTOF_H_
