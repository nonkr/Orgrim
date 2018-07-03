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
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s file blocksize\n", argv[0]);
        return 1;
    }

    char       *file          = argv[1];
    int        block_size     = strtol(argv[2], nullptr, 0);
    streamoff  nTotalSize;
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

    nTotalSize = fp.tellg();
#if __x86_64__
    printf("total file size:[%lu]\n", nTotalSize);
#else
    printf("total file size:[%llu]\n", nTotalSize);
#endif

    double dTotalDuration = 0;
    double dAvgDuration   = 0;

    fp.seekg(0, ios::beg);
    do
    {
        memset(buff, 0x00, (size_t) block_size);
        auto StartTime = system_clock::now();
        readSize = fp.read(buff, block_size).gcount();
        auto   EndTime      = system_clock::now();
        auto   DurationTime = duration_cast<microseconds>(EndTime - StartTime);
        double dDuration    = double(DurationTime.count()) * microseconds::period::num / microseconds::period::den;
        nTotalReadSize += readSize;
        if (readSize > 0)
        {
            buffLeft     = nTotalSize - (int) fp.tellg();
            frame++;
            dTotalDuration += dDuration;
            dAvgDuration = dTotalDuration / frame;
#if __x86_64__
            printf("frame:[%d/%.0lf] readSize:[%ld] nTotalReadSize:[%lu] buffLeft:[%lu] dTotalDuration:[%fs] dAvgDuration:[%fs]\n",
                   frame,
                   ceil(((float) nTotalSize / block_size)),
                   readSize,
                   nTotalReadSize,
                   buffLeft,
                   dTotalDuration,
                   dAvgDuration
            );
#else
            printf("frame:[%d/%.0lf] readSize:[%d] nTotalReadSize:[%llu] buffLeft:[%llu] dTotalDuration:[%fs] dAvgDuration:[%fs]\n",
                   frame,
                   ceil(((float) nTotalSize / block_size)),
                   readSize,
                   nTotalReadSize,
                   buffLeft,
                   dTotalDuration,
                   dAvgDuration
            );
#endif
        }
    } while (readSize > 0);

    fp.close();

    delete buff;

    return 0;
}
