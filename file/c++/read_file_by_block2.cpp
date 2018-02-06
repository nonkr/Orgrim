/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
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

    char       *file          = argv[1];
    int        block_size     = strtol(argv[2], nullptr, 0);
    streamoff  size;
    streamoff  buffLeft;
    streamsize readSize;
    streamoff  nTotalReadSize = 0;
    int        frame          = 0;
    ifstream   fp(file, ios::binary | ios::ate);
    auto       *buff          = new char[block_size];

    if (!fp.is_open())
    {
        fprintf(stderr, "open %s failed\n", file);
        return 2;
    }

    size = fp.tellg();
    printf("total file size:[%llu]\n", size);

    fp.seekg(0, ios::beg);
    do
    {
        memset(buff, 0x00, (size_t) block_size);
        readSize = fp.read(buff, block_size).gcount();
        nTotalReadSize += readSize;
        if (readSize > 0)
        {
            buffLeft = size - (int) fp.tellg();
            frame++;
            printf("frame:[%d] readSize:[%d] nTotalReadSize:[%llu] buffLeft:[%llu]\n",
                   frame,
                   readSize,
                   nTotalReadSize,
                   buffLeft);
        }
    } while (readSize > 0);

    fp.close();

    delete buff;

    return 0;
}
