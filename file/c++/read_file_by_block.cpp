/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/18 15:08
 *
 */

#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s file blocksize\n", argv[0]);
        return 1;
    }

    char     *file      = argv[1];
    int      block_size = strtol(argv[2], nullptr, 0);
    int      size;
    int      buffLeft;
    int      readSize;
    ifstream fp(file, ios::binary);
    auto     *buff      = new char[block_size];

    if (!fp.is_open())
    {
        fprintf(stderr, "open %s failed\n", file);
        return 2;
    }

    fp.seekg(0, ios::end);
    size = (int) fp.tellg();
    printf("file size:[%d]\n", size);

    fp.seekg(0, ios::beg);
    while (!fp.eof())
    {
        memset(buff, 0x00, (size_t) block_size);
        readSize = fp.read(buff, block_size).gcount();
        if (readSize > 0)
        {
            buffLeft = size - (int) fp.tellg();
            printf("readSize:[%d] buffLeft:[%d]\n", readSize, buffLeft);
            printf("[[%s]]\n", buff);
        }
    }

    fp.close();

    delete buff;

    return 0;
}
