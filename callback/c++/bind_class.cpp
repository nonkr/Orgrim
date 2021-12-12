/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2020/6/22 10:29
 *
 */

#include <iostream>
#include <functional>

using namespace std;

typedef std::function<void(int)> fpi;

class A;

class B
{
public:
    void SetCallback(fpi fptr)
    {
        f = fptr;
    }

    void Call()
    {
        f(1);
    }

private:
    fpi f;
};

class A
{
public:
    void Init()
    {
        m_pB = new B;
        m_pB->SetCallback(std::bind(&A::A_fun_int, this, std::placeholders::_1));
    }

    void A_fun_int(int i)
    {
        cout << "A_fun_int() " << i << endl;
    }

    void CallA()
    {
        m_pB->Call();
    }

private:
    B *m_pB = nullptr;
};

int main()
{
    A *pA = new A;
    pA->Init();
    pA->CallA();
    return 0;
}
