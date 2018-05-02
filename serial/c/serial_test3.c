/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/28 20:20
 *
 */

#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int  nUsartfd;
    char *pDevice   = "/dev/ttyS1";
    char *pSendStr  = "this a hello world text";

    nUsartfd = open(pDevice, O_RDWR | O_NOCTTY | O_NDELAY);
    if (nUsartfd == -1)
    {
        perror("open serial failed");
        return -1;
    }

    int iWriteLen = write(nUsartfd, pSendStr, strlen(pSendStr));
    printf("Send Len:[%d]\n", iWriteLen);
    printf("Send Str:[%s]\n", pSendStr);


    close(nUsartfd);
    return 0;
}
