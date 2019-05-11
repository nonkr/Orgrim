/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/4/22 18:01
 *
 */

#include <cstdio>
#include <fstream>

int main(int argc, char *argv[])
{
    std::ifstream t(argv[1], std::ifstream::ate);
    std::string str;

    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    t.close();

    printf("%s", str.c_str());

    return 0;
}
