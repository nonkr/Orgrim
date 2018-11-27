/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/22 10:55
 *
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <termios.h>
#include <stdio.h>

__BEGIN_DECLS

int speed_arr[] = {B4000000, B3500000, B3000000, B2500000, B2000000, B1500000, B1000000, B921600, B576000, B500000,
                   B460800, B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200,
                   B9600, B4800, B2400, B1200, B300,};
int name_arr[]  = {4000000, 3500000, 3000000, 2500000, 2000000, 1500000, 1000000, 921600, 576000, 500000, 460800,
                   230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400,
                   1200, 300,};

typedef struct
{
    char *pDevice;
    int  nSpeed;
    int  nDatabits;
    int  nStopbits;
    char nParity;
}   SerialOpts;

static int open_serial(SerialOpts *pSerialOpts) __attribute__((__unused__));

static void close_serial(int fd) __attribute__((__unused__));

static int set_speed(int fd, int nSpeed) __attribute__((__unused__));

static int set_Parity(int fd, int nDatabits, int nStopbits, char nParity) __attribute__((__unused__));

static int open_serial(SerialOpts *pSerialOpts)
{
    int fd;

    fd = open(pSerialOpts->pDevice, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        perror("open serial failed");
        return -1;
    }
    if (set_speed(fd, pSerialOpts->nSpeed) < 0)
    {
        printf("set_speed failed\n");
        return -1;
    }
    if (set_Parity(fd, pSerialOpts->nDatabits, pSerialOpts->nStopbits, pSerialOpts->nParity) < 0)
    {
        printf("set_Parity failed\n");
        return -1;
    }

    return fd;
}

static void close_serial(int fd)
{
    close(fd);
}

static int set_speed(int fd, int nSpeed)
{
    unsigned int   i;
    struct termios Opt;

    if (tcgetattr(fd, &Opt))
    {
        perror("tcgetattr");
        return -1;
    }
    for (i = 0; i < (sizeof(speed_arr) / sizeof(speed_arr[0])); i++)
    {
        if (nSpeed == name_arr[i])
        {
            if (tcflush(fd, TCIOFLUSH))
            {
                perror("tcflush");
                return -1;
            }
            if (cfsetispeed(&Opt, speed_arr[i]))
            {
                perror("cfsetispeed");
                return -1;
            }
            if (cfsetospeed(&Opt, speed_arr[i]))
            {
                perror("cfsetospeed");
                return -1;
            }
            if (tcsetattr(fd, TCSANOW, &Opt))
            {
                perror("tcsetattr");
                return -1;
            }
            if (tcflush(fd, TCIOFLUSH))
            {
                perror("tcflush");
                return -1;
            }
            break;
        }
    }
    return 0;
}

static int set_Parity(int fd, int nDatabits, int nStopbits, char nParity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("tcgetattr");
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (nDatabits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return -1;
    }
    switch (nParity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;    /* Clear parity enable */
            options.c_iflag &= ~INPCK;    /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;    /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;    /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;    /* Disnable parity checking */
            break;
        case 'S':
        case 's':            /*as no parity */
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return -1;
    }

    switch (nStopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return -1;
    }

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    options.c_oflag &= ~OPOST;                          /*Output*/

    // reference: https://linux.die.net/man/3/cfmakeraw
    cfmakeraw(&options);

    /* Set input parity option */
    if (nParity != 'n')
        options.c_iflag |= INPCK;
    if (tcflush(fd, TCIFLUSH))
    {
        perror("tcflush");
        return -1;
    }
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN]  = 0;    /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options))
    {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

__END_DECLS

#endif //_SERIAL_H_
