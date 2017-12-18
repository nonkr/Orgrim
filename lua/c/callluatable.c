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

static void print_stacknum(const char *desc, const int count)
{
    printf("%s: stack num = %d\n", desc, count);
}

int main(void)
{
    // 新建虚拟机
    L = luaL_newstate();

    // 载入Lua标准库
    luaL_openlibs(L);

    if (luaL_dofile(L, "test.lua"))
    {
        bail(L);
    }

    // 从Lua里面取得me这个table，并压入栈
    lua_getglobal(L, "me");
    if (!lua_istable(L, -1))
    {
        fprintf(stderr, "error! me is not a table");
    }
    print_stacknum("stage1", lua_gettop(L));

    stackPrettyDump(L);

    // 往栈里面压入一个key:name
    lua_getfield(L, -1, "name");
    printf("name = %s\n", lua_tostring(L, -1));
    print_stacknum("stage2", lua_gettop(L));

    stackPrettyDump(L);

    // 压入另一个key:age
    lua_getfield(L, -2, "age");
    printf("age = %lld\n", lua_tointeger(L, -1));
    print_stacknum("stage3", lua_gettop(L));

    stackPrettyDump(L);

    lua_close(L);
    return 0;
}
