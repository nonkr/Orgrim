/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct
{
    char school[20];
    int  grade;
    int  class;
} student;

typedef struct
{
    char    name[10];
    int     age;
    student st_student;
} child;

child arr_children[2] = {
    {
        "John",
        14,
        {
            "Santa Maria",
            4,
            6
        }
    },
    {
        "Lily",
        10,
        {
            "Santa Maria",
            1,
            5
        }
    }
};

void printHex(const char *data, int size)
{
    int i = 0;
    for (; i < size; i++)
    {
        printf("%02x", data[i] & 0xFF);
    }
    printf("\n");
}

static void set_children(child *p_children, int num)
{
    printf("sizeof:[%zd] num:[%d]\n", sizeof(p_children[0]), num);
    memcpy((void *) &p_children[0], &arr_children[0], sizeof(child));
    memcpy((void *) &p_children[1], &arr_children[1], sizeof(child));
}

int main()
{
    int   size = 2;
    static child pp[2];

    set_children(pp, size);

    printHex((char *) &pp[0], sizeof(child));
    printHex((char *) &pp[1], sizeof(child));

    return 0;
}
