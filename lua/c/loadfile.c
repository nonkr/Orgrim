/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/17 13:51
 *
 */

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "comm.h"

int main(void)
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    if (luaL_loadfile(L, "test.lua"))
    {
        bail(L);
    }

    stackPrettyDump(L);

    lua_close(L);
    return 0;
}
