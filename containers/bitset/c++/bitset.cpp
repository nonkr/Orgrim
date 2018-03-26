/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/23 13:33
 *
 */


#include <bitset>
#include <cstdio>

using std::bitset;

int main()
{
    bitset<8>  bs8;
    bitset<32> bs32("011000101011");

    bs8[0] = 1;
    bs8[3] = true;
    bs8[4] = true;
    bs8[7] = 1;

    printf("bs8:[%s]\n", bs8.to_string().c_str());
    printf("bs8 >> 1:[%s]\n", (bs8 >> 1).to_string().c_str());

    printf("bs8.size(): [%zu]\n", bs8.size());
    printf("bs8.any(): [%s]\n", bs8.any() ? "true" : "false");
    printf("bs8.none(): [%s]\n", bs8.none() ? "true" : "false");
    printf("bs8.count(): [%zu]\n", bs8.count());
    printf("bs8.flip(): [%s]\n", bs8.flip().to_string().c_str());
    printf("bs8.reset(): [%s]\n", bs8.reset().to_string().c_str());
    printf("bs8.set(0): [%s]\n", bs8.set(0).to_string().c_str());
    printf("bs8.set(1): [%s]\n", bs8.set(1).to_string().c_str());
    printf("bs8.set(): [%s]\n", bs8.set().to_string().c_str());
    printf("bs8.set(2, 0): [%s]\n", bs8.set(2, 0).to_string().c_str());
    printf("bs8.test(1): [%s]\n", bs8.test(1) ? "true" : "false");

    printf("bs32:[%s]\n", bs32.to_string().c_str());

    return 0;
}
