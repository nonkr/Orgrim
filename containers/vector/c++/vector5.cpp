/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/05/09 12:35
 *
 */

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

using std::vector;

struct Orange
{
    unsigned char id;
} __attribute__((__packed__));


int main()
{
    std::vector<Orange *> vOranges;

    Orange a = {0};
    Orange b = {0};
    Orange c = {0};

    a.id = 1;
    b.id = 2;
    c.id = 3;

    vOranges.emplace_back(&a);
    vOranges.emplace_back(&b);
    vOranges.emplace_back(&c);

    Orange *d = new Orange;
    d->id = 4;
    vOranges.push_back(d);

    printf("size:%zu\n", vOranges.size());

    for (Orange *it : vOranges)
    {
        printf("id:%d\n", it->id);
    }

    delete d;

    return 0;
}
