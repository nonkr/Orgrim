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
#include <exception>

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

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

struct People
{
    uint8_t  a;
    uint32_t b;
};

#pragma pack(push, 1)
struct Segment
{
    People   p;
    uint16_t magic;         // Magic Number
    uint16_t length;        // 包长(length字段之后的所有长度)
    uint32_t sn;            // Sequence Number
    uint32_t ack;           // Acknowledgment Number
    uint32_t sack;          // Shadow Acknowledgment Number
    uint16_t options;       // need ACK
    uint8_t  command;       // Command Type
    uint8_t  checksum;      // Checksum
    union
    {
        uint16_t a;
        uint16_t b;
    };
    int8_t   data[0];
};
#pragma pack(pop)

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

    printf("size of struct People:[%zu]\n", sizeof(People));
    printf("size of struct Segment:[%zu]\n", sizeof(Segment));

    Segment *s1 = (Segment *) malloc(sizeof(Segment) + sizeof(int8_t) * 3);
    printf("%zu\n", sizeof(*s1));
    s1->data[0] = 1;
    s1->data[1] = 2;
    s1->data[2] = 3;

    s1->a = 2;
    printf("s1->b:%d\n", s1->b);
    free(s1);

    int             ms = 1200;
    struct timespec stCondTimeout;
    clock_gettime(CLOCK_REALTIME, &stCondTimeout);
    stCondTimeout.tv_nsec = 948653564;

    struct tm *pTime1;
    pTime1 = localtime(&stCondTimeout.tv_sec);
    char sTemp1[32] = {0};
    snprintf(sTemp1, sizeof(sTemp1), "%04d-%02d-%02d %02d:%02d:%02d.%09ld", pTime1->tm_year + 1900,
             pTime1->tm_mon + 1, pTime1->tm_mday, pTime1->tm_hour, pTime1->tm_min, pTime1->tm_sec,
             stCondTimeout.tv_nsec);

    if ((long long) stCondTimeout.tv_nsec + ms * 1000000 >= 1000000000)
    {
        stCondTimeout.tv_sec += ((long long) stCondTimeout.tv_nsec + ms * 1000000) / 1000000000;
        stCondTimeout.tv_nsec = ((long long) stCondTimeout.tv_nsec + ms * 1000000) % 1000000000;
    }
    else
    {
        stCondTimeout.tv_nsec += ms * 1000000;
    }

    struct tm *pTime2;
    pTime2 = localtime(&stCondTimeout.tv_sec);
    char sTemp2[32] = {0};
    snprintf(sTemp2, sizeof(sTemp2), "%04d-%02d-%02d %02d:%02d:%02d.%09ld", pTime2->tm_year + 1900,
             pTime2->tm_mon + 1, pTime2->tm_mday, pTime2->tm_hour, pTime2->tm_min, pTime2->tm_sec,
             stCondTimeout.tv_nsec);
    printf("timedwait:[%s] -> [%s]\n", sTemp1, sTemp2);

    return 0;
}
