#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD; //CLOCAL:忽略modem控制线  CREAD：打开接受者
    newtio.c_cflag &= ~CSIZE; //字符长度掩码。取值为：CS5，CS6，CS7或CS8

    switch (nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
    }

    switch (nEvent)
    {
        case 'O':
            newtio.c_cflag |= PARENB; //允许输出产生奇偶信息以及输入到奇偶校验
            newtio.c_cflag |= PARODD;  //输入和输出是奇及校验
            newtio.c_iflag |= (INPCK | ISTRIP); // INPACK:启用输入奇偶检测；ISTRIP：去掉第八位
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
    }

    switch (nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        case 1500000:
            cfsetispeed(&newtio, B1500000);
            cfsetospeed(&newtio, B1500000);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            printf("Baud rate %d not support, set to default baud rate 9600\n", nSpeed);
            break;
    }

    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB; //CSTOPB:设置两个停止位，而不是一个
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;

//    cfmakeraw(&newtio);

    newtio.c_cc[VTIME] = 0; //VTIME:非cannoical模式读时的延时，以十分之一秒位单位
    newtio.c_cc[VMIN]  = 0; //VMIN:非canonical模式读到最小字符数
    tcflush(fd, TCIFLUSH); // 改变在所有写入 fd 引用的对象的输出都被传输后生效，所有已接受但未读入的输入都在改变发生前丢弃。
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) //TCSANOW:改变立即发生
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n\r");
    return 0;
}

int main(void)
{
    int  fd1, nset, nread, ret;
    char buf[100] = {"test com data!...........\n"};
    char buf1[1000];

    fd1 = open("/dev/ttyS1", O_RDWR);
    if (fd1 == -1)
        exit(1);
    printf("open  SAC0 success!!\n");

    nset = set_opt(fd1, 460800, 8, 'N', 1);
    if (nset == -1)
        exit(1);
    printf("SET SAC0 success!!\n");
    printf("enter the loop!!\n");

    while (1)
    {
        memset(buf1, 0, sizeof(buf1));
#if 0
        ret = write(fd1, buf, 100);
        if( ret > 0){
            printf("write success!  wait data receive\n");
        }
#endif
        nread = read(fd1, buf1, 1000);
        if (nread > 36)
        {
            printf("redatad: nread = %d\n", nread);
        }
//        sleep(1);
        //nread = read(fd1, buf1,1);
        //if(buf1[0] == 'q')
        //break;
    }
    close(fd1);

    return 0;
}
