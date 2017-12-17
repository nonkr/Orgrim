/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/17 15:51
 *
 */

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "comm.h"

static lua_State *L = NULL;

int ladd(int x, int y)
{
    int sum;

    lua_getglobal(L, "add");
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_call(L, 2, 1);

    sum = (int) lua_tointeger(L, -1);

    lua_pop(L, 1);

    return sum;
}

int main(void)
{
    L = luaL_newstate();

    luaL_openlibs(L);

    if (luaL_dofile(L, "test.lua"))
    {
        bail(L);
    }

    int sum = ladd(10, 20);

    printf("sum:[%d]\n", sum);

    lua_close(L);
    return 0;
}
