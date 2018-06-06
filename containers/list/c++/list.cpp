/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/9 15:11
 *
 */

#include <string>
#include <list>
#include <iostream>
#include <algorithm>

using namespace std;

void PrintIt(string &StringToPrint)
{
    cout << StringToPrint << endl;
}

int main()
{
    list<string> lst;

    if (lst.empty())
    {
        cout << "empty list" << endl;
    }

    lst.push_back("b");
    lst.push_back("c");
    lst.push_front("a");

    cout << "list size: " << lst.size() << endl;
    cout << "list max_size: " << lst.max_size() << endl;

    cout << "========" << endl;
    // 遍历方式一
    for (auto n : lst)
    {
        cout << n << endl;
    }
    cout << "========" << endl;

    // 遍历方式二
    list<string>::iterator lstIterator;
    for (lstIterator = lst.begin(); lstIterator != lst.end(); lstIterator++)
    {
        cout << *lstIterator << endl;
    }
    cout << "========" << endl;

    // 把list的元素倒转
    lst.reverse();

    // 遍历方式三
    for_each(lst.begin(), lst.end(), PrintIt);
    cout << "========" << endl;


    list<int> Scores;
    Scores.push_back(15);
    Scores.push_back(100);
    Scores.push_back(5);
    Scores.push_back(25);
    Scores.push_back(10);
    Scores.push_back(100);
    Scores.push_back(25);
    Scores.push_back(30);
    Scores.push_back(20);
    cout << count(Scores.begin(), Scores.end(), 100) << endl;

    cout << count_if(Scores.begin(), Scores.end(), [](int i) { return i % 10 == 0; }) << endl;

    cout << "========" << endl;
    for (auto n : Scores)
    {
        cout << n << endl;
    }

    Scores.sort();
    cout << "==== after sort() ====" << endl;
    for (auto n : Scores)
    {
        cout << n << endl;
    }

    Scores.erase(Scores.begin());
    cout << "==== after erase(Scores.begin()) ====" << endl;
    for (auto n : Scores)
    {
        cout << n << endl;
    }
    cout << "========" << endl;

    Scores.remove(25);
    cout << "==== after remove(25) ====" << endl;
    for (auto n : Scores)
    {
        cout << n << endl;
    }
    cout << "========" << endl;

    Scores.unique();
    cout << "==== after unique() ====" << endl;
    for (auto n : Scores)
    {
        cout << n << endl;
    }
    cout << "========" << endl;

    Scores.erase(Scores.begin(), Scores.end());
    cout << "==== after erase() ====" << endl;
    for (auto n : Scores)
    {
        cout << n << endl;
    }
    cout << "========" << endl;

    return 0;
}
