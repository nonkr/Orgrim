/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/17 21:32
 *
 */

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int averageFunc(lua_State *L)
{
    int n = lua_gettop(L);
    double sum = 0;
    int i;

    /* 循环求参数之和 */
    for (i = 1; i <= n; i++)
        sum += lua_tonumber(L, i);

    lua_pushnumber(L, sum / n);     //压入平均值
    lua_pushnumber(L, sum);         //压入和

    return 2;                       //返回两个结果
}

static int sayHelloFunc(lua_State *L)
{
    printf("hello world!\n");
    return 0;
}

static const struct luaL_Reg myLib[] =
        {
                {"average",  averageFunc},
                {"sayHello", sayHelloFunc},
                {NULL, NULL}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
        };

int luaopen_myLualib(lua_State *L)
{
#if LUA_VERSION_NUM >= 502
    lua_newtable(L);
    // 先把一个table压入VS，然后在调用luaL_setfuncs就会把所以的func存到table中
    // 注意不像luaL_register这个table是个无名table，可以在的使用只用一个变量来存入这个table。
    // e.g local clib = require "libname". 这样就不会污染全局环境。比luaL_register更好。
    luaL_setfuncs(L, myLib, 0);
#else
    luaL_register(L, "foo", myLib);
#endif
    return 1;       // 把myLualib表压入了栈中，所以就需要返回1
}
