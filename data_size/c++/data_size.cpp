/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/18 17:09
 *
 */

#include <fstream>
#include <climits>
#include "pack.h"

using namespace std;

enum SWITCH1
{
    SWITCH1_OPEN,
};

enum SWITCH2 : unsigned char
{
    SWITCH2_OPEN,
};

int main(int argc, char *argv[])
{
    printf("sizeof(char):%zu\t\tchar_max:%d\t\t\tuchar_max:%d\n", sizeof(char), SCHAR_MAX, UCHAR_MAX);
    printf("sizeof(short):%zu\t\tshort_max:%d\t\t\tushort_max:%d\n", sizeof(short), SHRT_MAX, USHRT_MAX);
    printf("sizeof(int):%zu\t\tint_max:%d\t\tuint_max:%u\n", sizeof(int), INT_MAX, UINT_MAX);
    printf("sizeof(long):%zu\t\tlong_max:%ld\t\tulong_max:%lu\n", sizeof(long), LONG_MAX, ULONG_MAX);
    printf("sizeof(long long):%zu\tllong_max:%lld\tullong_max:%llu\n", sizeof(long long), LLONG_MAX, ULLONG_MAX);
    printf("sizeof(float):%zu\n", sizeof(float));
    printf("sizeof(double):%zu\n", sizeof(double));
    printf("sizeof(void*):%zu\n", sizeof(void *));
    printf("sizeof(size_t):%zu\n", sizeof(size_t));

    printf("sizeof(normal enum):%zu\n", sizeof(SWITCH1_OPEN));
    printf("sizeof(unsigned char enum):%zu\n", sizeof(SWITCH2_OPEN));

    printf("size of streampos:[%zu]\n", sizeof(streampos));
    printf("size of streamsize:[%zu]\n", sizeof(streamsize));
    printf("size of streamoff:[%zu]\n", sizeof(streamoff));

    printf("size of struct PeoplePacked:[%zu]\n", sizeof(PeoplePacked));
    printf("size of struct PeopleUnPacked:[%zu]\n", sizeof(PeopleUnPacked));
    return 0;
}
