/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/22 17:01
 *
 */

#include <cstdio>
#include <exception>

class ModuleB
{
public:
    ModuleB()
    {
        printf("ModuleB init...\n");
        throw std::exception();
    }
};

class ModuleA
{
public:
    ModuleA()
    {
        printf("ModuleA init...\n");
        try
        {
            auto *pModuleB __attribute__((__unused__)) = new ModuleB;
        }
        catch (...)
        {
            printf("ModuleB init failed...\n");
            throw std::exception();
        }
    }
};


int main()
{
    printf("main...\n");
    try
    {
        auto *pModuleA __attribute__((__unused__)) = new ModuleA;
    }
    catch (...)
    {
        printf("ModuleA init failed...\n");
        return 1;
    }

    return 0;
}
