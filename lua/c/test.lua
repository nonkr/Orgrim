--
-- Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
-- All rights reserved.
--
-- This file is under MIT, see LICENSE for details.
--
-- Author: Billie Soong <nonkr@hotmail.com>
-- Datetime: 2017/12/17 13:47
--
local foo = "foo"

os.execute("sleep 3")

print("Hello World " .. foo)

me = {
    name = "Song",
    age = 29
}

students = {
    {
        name = "John",
        age = 23
    },
    {
        name = "Maya",
        age = 20
    },
    me
}

function add(x, y)
    return x + y
end
