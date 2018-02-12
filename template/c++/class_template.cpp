/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/2/6 11:12
 *
 */

namespace N
{
    template<class T>
    class Y
    {
        void mf() {}
    }; // 模板定义
}

// template class Y<int>; // 错误：类模板 Y 在全局命名空间不可见

// using N::Y;
// template class Y<int>; // 错误：显式实例化在模板的命名空间之外

template class N::Y<char *>;      // OK ：显式实例化
template void N::Y<double>::mf(); // OK ：显式实例化

int main()
{
    return 0;
}