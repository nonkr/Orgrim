/**  @ionbuffer.c
  *  @note Hikvision Software Co.Ltd. All Rights Reserved.
  *  @brief apply memory by ion
  *
  *  @author gaofeixue
  *  @date 2017/11/1
  *
  *  @note Version 1.0 Video Capture
  */

#include "ionbuffer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/ion.h>
#include <ion/ion.h>
#include <sys/mman.h>

static int ion_client = -1;
static int ion_buffer_count = 0;

/**  @fn     void free_ion(struct IonBuffer *ion);
  *  @brief  <release memory of buffer>
  *  @param  [in]  *ion   :the pointer points to struct IonBuffer
  *  @return No return
  */

//static void free_ion(struct IonBuffer *ion)
void free_ion(struct IonBuffer *ion)
{
    munmap(ion->vaddr, ion->mem_size);
    close(ion->fd);
    ion_free(ion->client, ion->handle);
    if(--ion_buffer_count <= 0)
        ion_close(ion->client);
}

/**  @fn     struct IonBuffer *AllocIonBuffer(int width, int height);
  *  @brief  <Apply memory by ion >
  *  @param  [in]  width    : the width  of Image resolution
  *  @param  [in]  height   : the height of Image resolution
  *  @return                   struct IonBuffer *AllocIonBuffer
  */

struct IonBuffer *AllocIonBuffer(int width, int height)
{
    struct IonBuffer *ion = malloc(sizeof(*ion));
    int ret, size = ((width + 0xf) & ~0xf) * ((height + 0xf) & ~0xf) * 3 / 2;

    if(!ion) 
    {
        printf("%s malloc buffer failed!!!\n", __func__);
        return NULL;
    }
    memset(ion, 0, sizeof(*ion));
    ion->width = width; ion->height = height;

    if(ion_client == -1) 
    {
        ion_client = ion_open();
        if(ion->client < 0) {
            printf("fail to open ion");
            goto ion_err;
        }
    }

    ion->client = ion_client;

    ret = ion_alloc(ion->client, size, 0, ION_HEAP_TYPE_DMA_MASK, 0, &ion->handle);
    if(ret) {
        printf("ion alloc %d bytes failed!\n", size);
        goto ion_err;
    }

    ret = ion_share(ion->client, ion->handle, &ion->fd);
    if(ret) 
    {
        printf("ion share return error\n");
        goto ion_err;
    }

    ion->vaddr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, ion->fd, 0);
    if(ion->vaddr == NULL) 
    {
        printf("mmap return failed!\n");
        goto ion_err;
    }

    ion->mem_size = size;
    ion_buffer_count++;
    ion->free = free_ion;

    return ion;

ion_err:
    free(ion);
    return NULL;
}
