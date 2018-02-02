/*
 * 使用升序定时器链表处理非活动连接。
 * 服务器程序通常要定期处理非活动连接：给客户端发一个重连请求，或者关闭该连接，或者其他。Linux在内核中提供了对连接是否处于活动状态的定
 * 期检查机制，我们可以通过socket选项KEEPALIVE来激活它。不过使用这种方式将使得应用程序对连接的管理变得复杂。因此，我们可以考虑在应用
 * 层实现类似于KEEPALIVE的机制，以管理所有长时间处于非活动状态的连接。例如，本程序利用alarm函数周期性地触发SIGALRM信号，该信号的信号
 * 处理函数利用管道通知主循环执行定时器链表上的定时任务——关闭非活动的连接。
 *
 * Copy from Linux高性能服务器编程 11-3
 * Datetime: 2018/2/2 17:29
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "lst_timer.h"

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 5

static int            pipefd[2];
static sort_timer_lst timer_lst;
static int            epollfd = 0;

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd)
{
    static epoll_event event;
    event.data.fd = fd;
    event.events  = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void sig_handler(int sig)
{
    int save_errno = errno;
    int msg        = sig;
    send(pipefd[1], (char *) &msg, 1, 0);
    errno          = save_errno;
}

void addsig(int sig)
{
    static struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, nullptr) != -1);
}

void timer_handler()
{
    /*
     * 定时处理任务，实际上就是调用tick函数
     */
    timer_lst.tick();

    /*
     * 因为一次alrm调用只会引起一次SIGALRM信号，所以我们要重新定时，以不断触发SIGALRM信号
     */
    alarm(TIMESLOT);
}

/**
 * 定时器回调函数，它删除非活动连接socket上的注册时间，并关闭之
 * @param user_data
 */
void cb_func(client_data *user_data)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    printf("close fd %d\n", user_data->sockfd);
}

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip  = argv[1];
    int        port = atoi(argv[2]);

    int                ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr *) &address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];

    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd);

    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]);
    addfd(epollfd, pipefd[0]);

    // add all the interesting signals here
    addsig(SIGALRM);
    addsig(SIGTERM);

    auto *users  = new client_data[FD_LIMIT];
    bool timeout = false;
    alarm(TIMESLOT);

    bool stop_server = false;
    while (!stop_server)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((number < 0) && (errno != EINTR))
        {
            printf("epoll failure\n");
            break;
        }

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) // 处理新到的客户连接
            {
                struct sockaddr_in client_address;
                socklen_t          client_addrlength = sizeof(client_address);
                int                connfd            = accept(listenfd, (struct sockaddr *) &client_address,
                                                              &client_addrlength);
                addfd(epollfd, connfd);
                users[connfd].address = client_address;
                users[connfd].sockfd  = connfd;

                /*
                 * 创建定时器，设置其回调函数与超时时间，然后绑定定时器与用户数据，最后将定时器添加到链表timer_lst中
                 */
                auto *timer = new util_timer;
                timer->user_data = &users[connfd];
                timer->cb_func   = cb_func;
                time_t cur = time(nullptr);
                timer->expire       = cur + 3 * TIMESLOT;
                users[connfd].timer = timer;
                timer_lst.add_timer(timer);
            }
            else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN)) // 处理信号
            {
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if (ret == -1)
                {
                    // handle the error
                    continue;
                }
                else if (ret == 0)
                {
                    continue;
                }
                else
                {
                    for (int j = 0; j < ret; ++j)
                    {
                        switch (signals[j])
                        {
                            case SIGALRM:
                            {
                                /*
                                 * 用timeout变量标记有定时任务需要处理，但不立即处理定时任务。这是因为定时任务的优先级不是很高，我
                                 * 们优先处理其他更重要的任务
                                 */
                                timeout = true;
                                break;
                            }
                            case SIGTERM:
                            {
                                stop_server = true;
                            }
                            default:;
                        }
                    }
                }
            }
            else if (events[i].events & EPOLLIN) // 处理客户连接上接收到的数据
            {
                memset(users[sockfd].buf, '\0', BUFFER_SIZE);
                ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE - 1, 0);
                printf("get %d bytes of client data %s from %d\n", ret, users[sockfd].buf, sockfd);
                util_timer *timer = users[sockfd].timer;
                if (ret < 0)
                {
                    // 如果发生读错误，则关闭连接，并移除其对应的定时器
                    if (errno != EAGAIN)
                    {
                        cb_func(&users[sockfd]);
                        if (timer)
                        {
                            timer_lst.del_timer(timer);
                        }
                    }
                }
                else if (ret == 0)
                {
                    // 如果对方已经关闭连接，则我们也关闭连接，并移除其对应的定时器
                    cb_func(&users[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
                else
                {
                    /*
                     * 如果某个客户连接上有数据可读，则我们要调整该连接对应的定时器，以延迟该连接被关闭的时间
                     */
                    if (timer)
                    {
                        time_t cur = time(nullptr);
                        timer->expire = cur + 3 * TIMESLOT;
                        printf("adjust timer once\n");
                        timer_lst.adjust_timer(timer);
                    }
                }
            }
            else
            {
                // others
            }
        }

        /*
         * 最后处理定时事件，因为I/O事件有更高的优先级。当然，这样做将导致定时任务不能精确地按照预期的时间执行
         */
        if (timeout)
        {
            timer_handler();
            timeout = false;
        }
    }

    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
    delete[] users;
    return 0;
}
