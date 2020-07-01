/*
 * Copyright (c) 2020, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2020/3/28 14:55
 *
 */

#include <cstring>
#include <string>
#include <list>

using namespace std;

struct People
{
    char name[32];
    int  age;
};

int main()
{
    list<People *> lst;

    auto *pPeople1 = new People;
    sprintf(pPeople1->name, "John");
    lst.emplace_back(pPeople1);

    auto *pPeople2 = new People;
    sprintf(pPeople2->name, "Bob");
    lst.emplace_back(pPeople2);

    for (auto i = lst.begin(); i != lst.end();)
    {
        printf("remove %s \n", (*i)->name);
        delete (*i);
        lst.remove(*i++);
    }

//    for (People *n : lst)
//    {
//        if (strcmp(n->name, "Bob") == 0)
//        {
//            printf("delete name:[%s]\n", n->name);
//            lst.remove(n);
//            delete n;
//            break;
//        }
//    }

    for (People *n : lst)
    {
        printf("rest name:[%s]\n", n->name);
        delete n;
    }

    lst.clear();

    return 0;
}
