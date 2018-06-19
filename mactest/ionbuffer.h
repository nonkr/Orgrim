/**  @ionbuffer.c
  *  @note Hikvision Software Co.Ltd. All Rights Reserved.
  *  @brief apply memory by ion
  *
  *  @author gaofeixue
  *  @date 2017/11/1
  *
  *  @note Version 1.0 Video Capture
  */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include <ion/ion.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct IonBuffer 
{
    int valid_data_size;
    uint32_t flags;
    int width;
    int height;
    void* vaddr;
    size_t mem_size;
    // the time when the content of buffer update
    int client;
    int fd;
    unsigned long offset;  // phys
    ion_user_handle_t handle;

    void (*free)(struct IonBuffer *ion);
};

void free_ion(struct IonBuffer *ion);
struct IonBuffer *AllocIonBuffer(int width, int height);

#ifdef __cplusplus
}
#endif

#endif  // BUFFER_H_
