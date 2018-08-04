/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/3 23:06
 *
 */

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "../../../../color.h"
#include "threadpool.h"

#define DEBUG

const int TASK_QUEUE_MAX = 1000;


struct task_data
{
    void *(*work_routine)(void *);

    void *arg;
};


struct thread_pool
{
    // N worker threads.
    pthread_t *worker_threads;

    // A circular queue that holds tasks that are yet to be executed.
    struct task_data *task_queue;

    // Head and tail of the queue.
    int queue_head, queue_tail;

    // How many worker threads can we have.
    int max_threads;

    // How many tasks are scheduled for execution. We use this so that
    // we can wait for completion.
    int scheduled;

    pthread_mutex_t mutex;

    // A condition that's signaled on when we go from a state of no work
    // to a state of work available.
    pthread_cond_t work_available;

    // A condition that's signaled on when we don't have any more tasks scheduled.
    pthread_cond_t done;
};


int pool_get_max_threads(struct thread_pool *pool)
{
    return pool->max_threads;
}


struct task_thread_args
{
    struct thread_pool *pool;
    struct task_data   td;
};


void *worker_thread_func(void *pool_arg)
{
    OGM_PRINT_BLUE("[W] Starting work thread.\n");
    struct thread_pool *pool = (struct thread_pool *) pool_arg;

    while (1)
    {
        struct task_data picked_task;

        pthread_mutex_lock(&pool->mutex);

        while (pool->queue_head == pool->queue_tail)
        {
            OGM_PRINT_BLUE("[W] Empty queue. Waiting...\n");
            pthread_cond_wait(&pool->work_available, &pool->mutex);
        }

        assert(pool->queue_head != pool->queue_tail);
        OGM_PRINT_BLUE("[W] Picked: %d\n", pool->queue_head);
        picked_task = pool->task_queue[pool->queue_head % TASK_QUEUE_MAX];
        pool->queue_head++;

        // The task is scheduled.
        pool->scheduled++;

        pthread_mutex_unlock(&pool->mutex);

        // Run the task.
        picked_task.work_routine(picked_task.arg);

        pthread_mutex_lock(&pool->mutex);
        pool->scheduled--;

        if (pool->scheduled == 0)
        {
            pthread_cond_signal(&pool->done);
        }
        pthread_mutex_unlock(&pool->mutex);
    }
    return NULL;
}


void pool_add_task(struct thread_pool *pool, void *(*work_routine)(void *), void *arg)
{
    pthread_mutex_lock(&pool->mutex);
    OGM_PRINT_BLUE("[Q] Queueing one item.\n");
    if (pool->queue_head == pool->queue_tail)
    {
        pthread_cond_broadcast(&pool->work_available);
    }

    struct task_data task;
    task.work_routine = work_routine;
    task.arg          = arg;

    pool->task_queue[pool->queue_tail % TASK_QUEUE_MAX] = task;
    pool->queue_tail++;

    pthread_mutex_unlock(&pool->mutex);
}


void pool_wait(struct thread_pool *pool)
{
    OGM_PRINT_BLUE("[POOL] Waiting for completion.\n");
    pthread_mutex_lock(&pool->mutex);
    while (pool->scheduled > 0)
    {
        pthread_cond_wait(&pool->done, &pool->mutex);
    }
    pthread_mutex_unlock(&pool->mutex);
    OGM_PRINT_BLUE("[POOL] Waiting done.\n");
}


struct thread_pool *pool_init(int max_threads)
{
    struct thread_pool *pool = malloc(sizeof(struct thread_pool));

    pool->queue_head = pool->queue_tail = 0;
    pool->scheduled  = 0;
    pool->task_queue = malloc(sizeof(struct task_data) * TASK_QUEUE_MAX);

    pool->max_threads    = max_threads;
    pool->worker_threads = malloc(sizeof(pthread_t) * max_threads);

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->work_available, NULL);
    pthread_cond_init(&pool->done, NULL);

    for (int i = 0; i < max_threads; i++)
    {
        pthread_create(&pool->worker_threads[i], NULL, worker_thread_func, pool);
    }

    return pool;
}


void pool_destroy(struct thread_pool *pool)
{
    pool_wait(pool);

    for (int i = 0; i < pool->max_threads; i++)
    {
        pthread_detach(pool->worker_threads[i]);
    }

    free(pool->worker_threads);
    free(pool->task_queue);

    free(pool);
}
