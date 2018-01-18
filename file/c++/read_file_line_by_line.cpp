/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/1/18 14:53
 *
 */

#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    char     *file = argv[1];
    ifstream fp(file);

    if (!fp.is_open())
    {
        fprintf(stderr, "open %s failed\n", file);
        return 2;
    }

    for (string line; getline(fp, line);)
    {
        printf("%s\n", line.c_str());
    }
    fp.close();

    return 0;
}
