/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/17 15:15
 *
 */

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <memory.h>
#include "comm.h"

int main(void)
{
    const char *buff = "print 'hello world'";
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    if (luaL_loadbuffer(L, buff, strlen(buff), "foo"))
    {
        bail(L);
    }

    lua_pcall(L, 0, 0, 0);

    stackDump(L);

    lua_close(L);
    return 0;
}
