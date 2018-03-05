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

#include <unistd.h>
#include "../../../color.h"
#include "job.h"

int Job::init(int id)
{
    m_id = id;

    return 0;
}

void Job::process()
{
    OGM_PRINT_BLUE("Job::process %d...\n", m_id);

    sleep(3);

    OGM_PRINT_BLUE("Job::process %d done...\n", m_id);
}