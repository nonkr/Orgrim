/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/17 15:15
 *
 */

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "comm.h"

int main(void)
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    if (luaL_loadstring(L, "print 'hello world'"))
    {
        bail(L);
    }

    stackDump(L);

    lua_close(L);
    return 0;
}
