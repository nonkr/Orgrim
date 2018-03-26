/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/16 16:08
 *
 */

#include <set>
#include <cstdio>
#include <typeinfo>

using std::set;

class Student
{
public:
    Student() {};

    ~Student() {};

    int age;
};

int main()
{
    set<Student *> setStudents;

    auto *pStudent1 = new Student;
    pStudent1->age = 12;

    auto *pStudent2 = new Student;
    pStudent2->age = 20;

    printf("pStudent1: %s\n", typeid(pStudent1).name());
    printf("pStudent2: %s\n", typeid(pStudent2).name());

    setStudents.insert(pStudent1);
    setStudents.insert(pStudent2);

    for (auto n : setStudents)
    {
        printf("age: %d\n", n->age);
    }

    return 0;
}
