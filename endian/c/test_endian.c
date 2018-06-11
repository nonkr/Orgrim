/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/4/28 15:57
 *
 */

#include <stdio.h>
#include <netinet/in.h>
#include "../../utils/print_utils.h"

static float htonf(float value)
{
    union v
    {
        float        f;
        unsigned int i;
    };

    union v val;

    val.f = value;
    val.i = htonl(val.i);

    return val.f;
};

static float ntohf(float value)
{
    union v
    {
        float        f;
        unsigned int i;
    };

    union v val;

    val.f = value;
    val.i = ntohl(val.i);

    return val.f;
};

int main()
{
    union w
    {
        int  a; //4 bytes
        char b; //1 byte
    } c;

    c.a = 1;

    if (c.b == 1)
    {
        printf("Little endian\n");
    }
    else
    {
        printf("Big endian\n");
    }

    float f = 1.2345;
    print_as_hexstring((char *) &f, sizeof(f));

    float f2 = htonf(f);
    print_as_hexstring((char *) &f2, sizeof(f2));

    float f3 = ntohf(f2);
    print_as_hexstring((char *) &f3, sizeof(f3));

    unsigned short s = 2;
    print_as_hexstring((char *) &s, sizeof(s));

    unsigned short s2 = htons(s);
    print_as_hexstring((char *) &s2, sizeof(s2));

    unsigned short s3 = ntohs(s2);
    print_as_hexstring((char *) &s3, sizeof(s3));


    return 1;
}
