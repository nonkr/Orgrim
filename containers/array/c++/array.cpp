/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/9 15:52
 *
 */

#include <iostream>
#include <array>
#include <string>

using namespace std;

int main()
{
    array<string, 5> arr = {"C++", "Java", "PHP"};

    cout << "arr.size() = " << arr.size() << endl;
    cout << "arr.max_size() = " << arr.max_size() << endl;

    if (!arr.empty())
    {
        cout << "arr is not empty" << endl;
    }

    string lang = arr.at(0);
    cout << "arr.at(0) = " << lang << endl;

    lang = arr.at(1);
    cout << "arr.at(1) = " << lang << endl;

    lang = arr.at(2);
    cout << "arr.at(2) = " << lang << endl;

    lang = arr.at(3);
    cout << "arr.at(3) = " << lang << endl;

    lang = arr.at(4);
    cout << "arr.at(4) = " << lang << endl;

    cout << "========" << endl;

    string *pStr = arr.data();
    for (size_t n = 0; n < arr.size(); n++, pStr++)
        cout << "arr.data() = " << *pStr << endl;

    return 0;
}
