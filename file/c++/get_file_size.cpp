/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/10 19:35
 *
 */

#include <cstdio>
#include <fstream>

int main(int argc, char *argv[])
{
    std::ifstream in(argv[1], std::ifstream::ate | std::ifstream::binary);
    printf("file size %d\n", (int) in.tellg());
    in.close();
    return 0;
}
