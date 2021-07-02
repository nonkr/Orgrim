#include <termios.h>
#include <fcntl.h>
#include "serial.h"
#include "../../../common/PrintUtil.h"

int speed_arr[] = {B1500000, B921600, B460800, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B38400, B19200, B9600, B4800, B2400, B1200, B300,};
int name_arr[]  = {1500000, 921600, 460800, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200,
                   9600, 4800, 2400, 1200, 300,};

int OpenTTY(const char *pDevice, bool bBlock)
{
    int fd = 0;
    if (bBlock)
    {
        fd = open(pDevice, O_RDWR);
    }
    else
    {
        fd = open(pDevice, O_RDWR | O_NONBLOCK);
    }

    return fd;
}

int SetBaud(int fd, int speed)
{
    unsigned char  i;
    int            status;
    struct termios Opt;
    tcgetattr(fd, &Opt);
    for (i = 0; i < (sizeof(speed_arr) / sizeof(speed_arr[0])); i++)
    {
        if (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0)
            {
                perror("tcsetattr fd1");
                return -1;
            }
            tcflush(fd, TCIOFLUSH);
        }
    }
    return 0;
}

int SetOpt(int fd, int dataBits, int stopBits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }

    //    reference: https://linux.die.net/man/3/cfmakeraw
//    cfmakeraw() sets the terminal to something like the "raw" mode of the old Version 7 terminal driver: input is
//    available character by character, echoing is disabled, and all special processing of terminal input and output
//    characters is disabled. The terminal attributes are set as follows:
//        termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
//                                | INLCR | IGNCR | ICRNL | IXON);
//        termios_p->c_oflag &= ~OPOST;
//        termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
//        termios_p->c_cflag &= ~(CSIZE | PARENB);
//        termios_p->c_cflag |= CS8;
    cfmakeraw(&options);

    options.c_cflag &= ~CSIZE;
    switch (dataBits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            FR_PRINT_RED("UART", "Unsupported data size\n");
            return -1;
    }
    switch (parity)
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
            FR_PRINT_RED("UART", "Unsupported parity\n");
            return -1;
    }

    switch (stopBits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            FR_PRINT_RED("UART", "Unsupported stop bits\n");
            return -1;
    }

    options.c_lflag &= ~(ICANON | ISIG);
    options.c_iflag &= ~(ICRNL | IGNCR);

    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 150;
    options.c_cc[VMIN]  = 0;    /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}
