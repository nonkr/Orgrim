/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/17 20:50
 *
 */

#ifndef ORGRIM_LUA_COMM_H
#define ORGRIM_LUA_COMM_H

#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

static void bail(lua_State *L)
{
    fprintf(stderr, "\nFATAL ERROR:%s\n\n", lua_tostring(L, -1));
    exit(1);
}

static void stackDump(lua_State *L) __attribute__((unused));

static void stackDump(lua_State *L)
{
    int i = 0;
    int type = 0;
    int top = lua_gettop(L);

    printf("There is %d elements in stack: ", top);
    for (i = 1; i <= top; i++)
    {
        type = lua_type(L, i);
        switch (type)
        {
            case LUA_TSTRING:
                printf("'%s'", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%g", lua_tonumber(L, i));
                break;
            default:
                printf("Element type is %s", lua_typename(L, type));
                break;
        }
        printf(" ");
    }
    printf("\n");
    return;
}

static void stackPrettyDump(lua_State *L) __attribute__((unused));
static void stackPrettyDump(lua_State *L)
{
    int i = 0;
    int type = 0;
    int top = lua_gettop(L);

    printf("There is %d elements in stack: ", top);
    for (i = top; i >= 1; i--)
    {
        if (i == top)
            printf("\n┏━━━━┳━━━━━━━━━━━━━━┳━━━━┓\n");
        printf("↑ %d  ┃", i);
        type = lua_type(L, i);
        switch (type)
        {
            case LUA_TSTRING:
                printf("'%s'", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%g", lua_tonumber(L, i));
                break;
            default:
                printf("<%s>", lua_typename(L, type));
                break;
        }
        printf("\033[21G┃ -%d ↓\n", top - i + 1);
        if (i == 1)
            printf("┗━━━━┻━━━━━━━━━━━━━━┻━━━━┛\n");
        else
            printf("┣━━━━╋━━━━━━━━━━━━━━╋━━━━┫\n");
    }
    printf("\n");
    return;
}

#endif //ORGRIM_LUA_COMM_H
