/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/2 15:11
 *
 */

#include <unistd.h>
#include <cassert>
#include "ThreadPool.h"
#include "job.h"

#define MAX_JOBS 5

int main()
{
    ThreadPool<Job> *pool = nullptr;
    try
    {
        pool = new ThreadPool<Job>(3);
    }
    catch (...)
    {
        return 1;
    }

    auto *jobs = new Job[MAX_JOBS];
    assert(jobs);

    for (int i = 0; i < MAX_JOBS; i++)
    {
        jobs[i].init(i);
        pool->append(&jobs[i]);
    }

    pool->stop();

    delete[] jobs;
    delete pool;
    return 0;
}