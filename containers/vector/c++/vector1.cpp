/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/25 15:04
 *
 */

#include <vector>
#include <iostream>
#include <string>

using std::vector;
using std::string;

union S
{
    string      str;
    vector<int> vec;

    ~S() {} // 需要知道哪个成员为活跃，仅在类联合类中可行
};          // 整个联合体占有 max(sizeof(string), sizeof(vector<int>))

int main()
{
    S s = {"Hello, world"};
    // 在此点，从 s.vec 读取是未定义行为
    std::cout << "s.str = " << s.str << '\n';
    s.str.~basic_string<char>();

    new(&s.vec) vector<int>;
    // 现在，s.vec 是联合体的活跃成员
    s.vec.push_back(10);
    std::cout << s.vec.size() << '\n';
    s.vec.~vector<int>();
}