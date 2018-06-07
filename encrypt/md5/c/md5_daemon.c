/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/6/7 9:39
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <openssl/md5.h>

int BinaryCompare(const unsigned char *pBin1, size_t sLen1, const unsigned char *pBin2, size_t sLen2)
{
    unsigned char digest1[16];
    unsigned char digest2[16];

    MD5(pBin1, sLen1, digest1);
    MD5(pBin2, sLen2, digest2);

    char md5string1[33];
    char md5string2[33];

    for (int i = 0; i < 16; ++i)
    {
        sprintf(&md5string1[i * 2], "%02x", (unsigned int) digest1[i]);
        sprintf(&md5string2[i * 2], "%02x", (unsigned int) digest2[i]);
    }

    printf("%s\n", md5string1);
    printf("%s\n", md5string2);

    return memcmp(digest1, digest2, sizeof(digest1));
}

int main()
{
    short a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    short b[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    if (BinaryCompare((unsigned char *) a, sizeof(a), (unsigned char *) b, sizeof(b)) == 0)
    {
        printf("same\n");
    }

    return 0;
}
