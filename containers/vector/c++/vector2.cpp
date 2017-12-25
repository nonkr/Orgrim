/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/25 15:35
 *
 */
#include <vector>
#include <iostream>

using std::vector;
using std::string;

struct subject
{
    string name;
    int    marks;
    int    credits;
};

int main()
{
    vector<subject> sub;

    //Push back new subject created with default constructor.
    sub.emplace_back(subject());

    //Vector now has 1 element @ index 0, so modify it.
    sub[0].name = "english";

    //Add a new element if you want another:
    sub.emplace_back(subject());

    //Modify its name and marks.
    sub[1].name  = "math";
    sub[1].marks = 90;

    struct subject su;
    su.name = "pt";
    su.marks = 20;
    su.credits = 30;
    sub.push_back(su);

    for (auto s : sub)
    {
        std::cout << s.name << std::endl;
    }
}