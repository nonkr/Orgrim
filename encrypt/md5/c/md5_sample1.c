/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/6/6 17:27
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <openssl/md5.h>

int main()
{
    unsigned char digest[16];
    const char    *string = "1";

    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, string, strlen(string));
    MD5_Final(digest, &context);

    char md5string[33];

    for (int i = 0; i < 16; ++i)
        sprintf(&md5string[i * 2], "%02x", (unsigned int) digest[i]);

    printf("%s\n", md5string);

    return 0;
}
