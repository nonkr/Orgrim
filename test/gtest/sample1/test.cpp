/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/12 17:00
 *
 */

#include "gtest/gtest.h"
#include "sample.h"

TEST(fun, case1)
{
    EXPECT_LT(-2, fun(1, 2));
    EXPECT_EQ(-1, fun(1, 2));
    ASSERT_LT(-2, fun(1, 2));
    ASSERT_EQ(-1, fun(1, 2));
}
