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

int main()
{
    unsigned char digest1[16];
    unsigned char digest2[16];
    short         a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    short         b[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    MD5((unsigned char *) a, sizeof(a), digest1);
    MD5((unsigned char *) b, sizeof(b), digest2);

    if (memcmp(digest1, digest2, sizeof(digest1)) == 0)
    {
        printf("same\n");
    }

    char md5string1[33];
    char md5string2[33];

    for (int i = 0; i < 16; ++i)
    {
        sprintf(&md5string1[i * 2], "%02x", (unsigned int) digest1[i]);
        sprintf(&md5string2[i * 2], "%02x", (unsigned int) digest2[i]);
    }

    printf("%s\n", md5string1);
    printf("%s\n", md5string2);

    return 0;
}
