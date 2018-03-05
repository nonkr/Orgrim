/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/2 15:07
 *
 */

#ifndef _JOB_H_
#define _JOB_H_

class Job
{
public:

    int init(int id);

    void process();

private:
    int m_id;
};

#endif //_JOB_H_
