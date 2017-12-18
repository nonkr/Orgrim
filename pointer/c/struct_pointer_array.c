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

    memcpy((void *) &p_children[0], arr_children, sizeof(arr_children));
}

int main()
{
    int   size = 2;
    child *p[size];
    p[0] = (child *) malloc(sizeof(child) * size);
    memset(p[0], 0x00, sizeof(child) * size);

    set_children(p[0], size);

    printHex((char *) p[0], sizeof(child));
    printHex((char *) p[0] + sizeof(child), sizeof(child));

    return 0;
}
