#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "serial.h"

int main(int argc, char **argv)
{
    SerialOpts stSerialOpts = {
        .nSpeed    = 115200,
        .nDatabits = 8,
        .nStopbits = 1,
        .nParity   = 'e',
        .pDevice   = "/dev/ttyUSB0"
    };

    int fd = -1;

    if ((fd = open_serial(&stSerialOpts)) < 0)
    {
        fprintf(stderr, "open serial failed\n");
        exit(1);
    }

    char buf[512];
    int  iReadLen;

    while (1)
    {
        iReadLen = read(fd, buf, 1);
        if (iReadLen > 0)
        {
            printf("Recv: %02X\n", buf[0]);
        }
        usleep(50000);
    }

    return 0;
}
