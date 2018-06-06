--
-- Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
-- All rights reserved.
--
-- This file is under MIT, see LICENSE for details.
--
-- Author: Billie Soong <nonkr@hotmail.com>
-- Datetime: 2017/12/17 21:48
--

local foo = require "myLualib"
local ave, sum = foo.average(1, 2, 3, 4, 5) -- 参数对应堆栈中的数据
print(ave, sum) -- 3 15
foo.sayHello() -- hello world!
