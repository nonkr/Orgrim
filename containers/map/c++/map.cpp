/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/16 16:23
 *
 */

#include <cstdio>
#include <map>
#include <typeinfo>
#include <string>

using std::map;
using std::string;

class Student
{
public:
    Student() {};

    ~Student() {};

    int age;
};

int main()
{
    map<string, Student *> mapStudents;

    auto *pStudent1 = new Student;
    pStudent1->age = 12;

    auto *pStudent2 = new Student;
    pStudent2->age = 20;

    printf("pStudent1: %s\n", typeid(*pStudent1).name());
    printf("pStudent2: %s\n", typeid(*pStudent2).name());

    mapStudents[typeid(*pStudent1).name()] = pStudent1;

    if (mapStudents.count(typeid(*pStudent1).name()) == 0)
    {
        mapStudents[typeid(*pStudent2).name()] = pStudent2;
    }

    for (const auto &n : mapStudents)
    {
        printf("age: %d\n", n.second->age);
    }

    delete pStudent1;
    delete pStudent2;

    return 0;
}
