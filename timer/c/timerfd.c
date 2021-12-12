#include <sys/timerfd.h>
#include <time.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

const int MAXNUM = 20;

int main(int argc, char *argv[])
{
    struct itimerspec new_value1;
    struct itimerspec new_value2;
    struct timespec   now;
    uint64_t          exp;
    ssize_t           s;

    int ret = clock_gettime(CLOCK_MONOTONIC, &now);//获取时钟时间
    assert(ret != -1);

    new_value1.it_value.tv_sec     = now.tv_sec + 2; //第一次到期的时间
    new_value1.it_value.tv_nsec    = 0;
    new_value1.it_interval.tv_sec  = 1;      //之后每次到期的时间间隔
    new_value1.it_interval.tv_nsec = 0;

    new_value2.it_value.tv_sec     = now.tv_sec + 3; //第一次到期的时间
    new_value2.it_value.tv_nsec    = 0;
    new_value2.it_interval.tv_sec  = 2;      //之后每次到期的时间间隔
    new_value2.it_interval.tv_nsec = 0;

    int epollfd = epoll_create(1);  //创建epoll实例对象

    struct epoll_event event;   // 告诉内核要监听什么事件
    struct epoll_event wait_event[MAXNUM];

    // CLOCK_REALTIME  相对时间，从1970.1.1到目前的时间。更改系统时间会更改获取的值。它以系统时间为坐标。
    // CLOCK_MONOTONIC 以绝对时间为准，获取的时间为系统重启到现在的时间，更改系统时间对它没有影响。
    int timefd1 = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK); // 构建了一个定时器
    assert(timefd1 != -1);
    ret = timerfd_settime(timefd1, TFD_TIMER_ABSTIME, &new_value1, NULL);       //启动定时器
    assert(ret != -1);
    printf("timer1 started, will triggerd in %lds\n", new_value1.it_value.tv_sec); // 定时器开启啦！
    event.data.fd = timefd1;
    event.events  = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timefd1, &event); //添加到epoll事件集合

    int timefd2 = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK); // 构建了一个定时器
    assert(timefd2 != -1);
    ret = timerfd_settime(timefd2, TFD_TIMER_ABSTIME, &new_value2, NULL);       //启动定时器
    assert(ret != -1);
    printf("timer2 started, will triggerd in %lds\n", new_value2.it_value.tv_sec); // 定时器开启啦！
    event.data.fd = timefd2;
    event.events  = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timefd2, &event); //添加到epoll事件集合

    int count = 0;

    for (;;)
    {
        // 监视并等待多个文件（标准输入，有名管道）描述符的属性变化（是否可读）
        // 没有属性变化，这个函数会阻塞，直到有变化才往下执行
        int num = epoll_wait(epollfd, wait_event, MAXNUM, 0);
        assert(num >= 0);
        for (int i = 0; i < num; i++)
        {
            if (wait_event[i].events & EPOLLIN)
            {
                //....处理其他事件
                if (wait_event[i].data.fd == timefd1)
                {
                    s = read(wait_event[i].data.fd, &exp, sizeof(uint64_t)); //需要读出uint64_t大小, 不然会发生错误
                    assert(s == sizeof(uint64_t));
                    printf("here is timer1, count:%d\n", count++);
                    if (count == 5)
                    {
                        printf("stop timer2\n");
                        new_value2.it_value.tv_sec  = 0; //it_value两个值都设成0表示关闭定时器
                        new_value2.it_value.tv_nsec = 0; //it_value两个值都设成0表示关闭定时器
                        timerfd_settime(timefd2, TFD_TIMER_ABSTIME, &new_value2, NULL);
                    }
                    else if (count == 10)
                    {
                        printf("restart timer2\n");
                        ret = clock_gettime(CLOCK_MONOTONIC, &now);//获取时钟时间
                        assert(ret != -1);
                        new_value2.it_value.tv_sec     = now.tv_sec + 2; //第一次到期的时间
                        new_value2.it_value.tv_nsec    = 0;
                        new_value2.it_interval.tv_sec  = 2;      //之后每次到期的时间间隔
                        new_value2.it_interval.tv_nsec = 0;
                        timerfd_settime(timefd2, TFD_TIMER_ABSTIME, &new_value2, NULL);
                    }
                }
                else if (wait_event[i].data.fd == timefd2)
                {
                    s = read(wait_event[i].data.fd, &exp, sizeof(uint64_t)); //需要读出uint64_t大小, 不然会发生错误
                    assert(s == sizeof(uint64_t));
                    printf("here is timer2\n");
                }
            }
        }
    }

    close(timefd1);
    close(timefd2);
    close(epollfd);

    return 0;
}
