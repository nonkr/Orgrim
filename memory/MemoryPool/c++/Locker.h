#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <exception>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

class barrier
{
public:
    barrier(int num)
    {
        pthread_barrier_init(&m_barrier, nullptr, num);
    }

    ~barrier()
    {
        pthread_barrier_destroy(&m_barrier);
    }

    void wait()
    {
        pthread_barrier_wait(&m_barrier);
    }

private:
    pthread_barrier_t m_barrier;
};

/**
 * 封装信号量的类
 */
class sem
{
public:
    /**
     * 创建并初始化信号量
     */
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            // 构造函数没有返回值，可以通过抛出异常来报告错误
            throw std::exception();
        }
    }

    /**
     * 销毁信号量
     */
    ~sem()
    {
        sem_destroy(&m_sem);
    }

    /**
     * 等待信号量
     * @return bool - successful
     */
    bool wait()
    {
        return sem_wait(&m_sem) == 0;
    }

    /**
     * 增加信号量
     * @return bool - successful
     */
    bool post()
    {
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

/**
 * 封装互斥锁的类
 */
class locker
{
public:
    /**
     * 创建并初始化互斥锁
     */
    locker()
    {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0)
        {
            throw std::exception();
        }
    }

    /**
     * 销毁互斥锁
     */
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    /**
     * 获取互斥锁
     * @return bool - successful
     */
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }


    /**
     * 释放互斥锁
     * @return bool - successful
     */
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

private:
    pthread_mutex_t m_mutex;
};

/**
 * 封装条件变量的类
 */
class cond
{
public:
    /**
     * 创建并初始化条件变量
     */
    cond()
    {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0)
        {
            throw std::exception();
        }
        if (pthread_cond_init(&m_cond, nullptr) != 0)
        {
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }

    /**
     * 销毁条件变量
     */
    ~cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    /**
     * 获取互斥锁
     * @return bool - successful
     */
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    /**
     * 释放互斥锁
     * @return bool - successful
     */
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    /**
     * 等待条件变量
     * @return bool - successful
     */
    bool wait()
    {
        return pthread_cond_wait(&m_cond, &m_mutex) == 0;
    }

    /**
     * 超时等待条件变量
     * @return int - the return of pthread_cond_timedwait
     */
    int timedwait(int seconds)
    {
        struct timespec stCondTimeout;
        clock_gettime(CLOCK_REALTIME, &stCondTimeout);
        stCondTimeout.tv_sec += seconds;
        return pthread_cond_timedwait(&m_cond, &m_mutex, &stCondTimeout);
    }

    /**
     * 锁定互斥变量并等待条件变量
     * @return bool - successful
     */
    bool lockwait()
    {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    /**
     * 锁定互斥变量并超时等待条件变量
     * @return int - the return of pthread_cond_timedwait
     */
    int locktimedwait(int iTimedOutSec)
    {
        int             ret = 0;
        struct timespec stCondTimeout;

        clock_gettime(CLOCK_REALTIME, &stCondTimeout);
        stCondTimeout.tv_sec += iTimedOutSec;

        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, &m_mutex, &stCondTimeout);
        pthread_mutex_unlock(&m_mutex);
        return ret;
    }

    /**
     * 唤醒等待条件变量的线程
     * @return bool - successful
     */
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }

    /**
     * 唤醒所有等待条件变量的线程
     * @return bool - successful
     */
    bool broadcast()
    {
        return pthread_cond_broadcast((&m_cond)) == 0;
    }

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
};


/**
 * 封装自旋锁的类
 */
class SpinLocker
{
public:
    /**
     * 创建并初始化自旋锁
     */
    SpinLocker()
    {
        if (pthread_spin_init(&m_spinlock, 0) != 0)
        {
            throw std::exception();
        }
    }

    /**
     * 销毁自旋锁
     */
    ~SpinLocker()
    {
        pthread_spin_destroy(&m_spinlock);
    }

    /**
     * 获取自旋锁
     * @return bool - successful
     */
    bool lock()
    {
        return pthread_spin_lock(&m_spinlock) == 0;
    }


    /**
     * 释放自旋锁
     * @return bool - successful
     */
    bool unlock()
    {
        return pthread_spin_unlock(&m_spinlock) == 0;
    }

private:
    pthread_spinlock_t m_spinlock;
};

#endif //_LOCKER_H_
