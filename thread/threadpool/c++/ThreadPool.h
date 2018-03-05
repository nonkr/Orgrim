#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include <unistd.h>
#include "locker.h"

/**
 * 线程池类，将它定义为模板类是为了代码复用。模板参数T是任务类
 * @tparam T
 */
template<typename T>
class ThreadPool
{
public:
    /**
     * @param thread_number 线程池中线程的数量
     * @param max_requests 请求队列中最多允许的、等待处理的请求的数量
     */
    ThreadPool(int thread_number = 8, unsigned int max_requests = 10000);

    ~ThreadPool();

    /**
     * 往请求队列中添加任务
     * @param request
     * @return
     */
    bool append(T *request);

    void join();

    void stop();

private:
    static void *worker(void *arg);

    void run();

private:
    int            m_thread_number;  // 线程池中的线程数
    unsigned int   m_max_requests;   // 请求队列中允许的最大请求数
    bool           m_stop;           // 是否结束线程
    pthread_t      *m_threads;       // 描述线程池的数组，其大小为m_thread_number
    std::list<T *> m_workqueue;      // 请求队列
    locker         m_queuelocker;    // 保护请求队列的互斥锁
    sem            m_queuestat;      // 是否有任务需要处理
    cond           m_emptyqueue_cond;
    bool           m_emptyqueue;
};

template<typename T>
ThreadPool<T>::ThreadPool(int thread_number, unsigned int max_requests) :
    m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false), m_threads(NULL)
{
    if ((thread_number <= 0) || (max_requests <= 0))
    {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
    {
        throw std::exception();
    }

    /*
     * 创建thread_number个线程，并将它们都设置为脱离线程
     */
    for (int i = 0; i < thread_number; i++)
    {
        printf("Create the %dth thread\n", i);
        if (pthread_create(m_threads + i, NULL, worker, this))
        {
            delete[] m_threads;
            throw std::exception();
        }
//        if (pthread_detach(m_threads[i]))
//        {
//            delete[] m_threads;
//            throw std::exception();
//        }
    }

    usleep(500000); // 0.5s
    printf("After sleep 0.5s and exit new ThreadPool\n");
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
    delete[] m_threads;
    m_stop = true;
}

template<typename T>
bool ThreadPool<T>::append(T *request)
{
    // 操作工作队列时一定要加锁，因为它被所有线程共享
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void ThreadPool<T>::join()
{
    for (int i = 0; i < m_thread_number; ++i)
    {
        printf("Join the %dth thread\n", i);
        pthread_join(m_threads[i], nullptr);
    }
}

template<typename T>
void ThreadPool<T>::stop()
{
    while(!m_workqueue.empty())
    {
        m_emptyqueue_cond.wait();
    }

    m_stop = true;
    for (int i = 0; i < m_thread_number; ++i)
    {
        m_queuestat.post();
    }

    join();
}

template<typename T>
void *ThreadPool<T>::worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *) arg;
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run()
{
    while (!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (!request)
        {
            continue;
        }
        request->process();

        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_emptyqueue_cond.signal();
        }
        m_queuelocker.unlock();
    }
}

#endif //_THREADPOOL_H_
