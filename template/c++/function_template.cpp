/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/2/6 13:41
 *
 */

#include <iostream>

template<typename T>
void f(T s)
{
    std::cout << s << '\n';
}

// 显式实例化
template void f<double>(double); // 实例化 f<double>(double)
template void f<>(char);         // 实例化 f<char>(char)，推导出模板实参
template void f(int);            // 实例化 f<int>(int)，推导出模板实参

// 模板实参推导。编译未通过，http://zh.cppreference.com/w/cpp/language/function_template
/*
template<typename To, typename From> To convert(From f);
void g(double d)
{
    int  i = convert<int>(d);    // 调用 convert<int,double>(double)
    char c = convert<char>(d);   // 调用 convert<char,double>(double)
    int (*ptr)(float) = convert; // 实例化 convert<int, float>(float)
}
*/

int main()
{
    // 隐式实例化
    f<double>(1); // 实例化并调用 f<double>(double)
    f<>('a');     // 实例化并调用 f<char>(char)
    f(7);         // 实例化并调用 f<int>(int)
    void (*ptr)(std::string) = f; // 实例化 f<string>(string)

    return 0;
}