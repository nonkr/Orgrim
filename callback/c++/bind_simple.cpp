/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/7/4 14:10
 *
 */

#include <cstdio>
#include <functional>

using namespace std;

typedef std::function<void(int, const unsigned char *, size_t)> fpi;

#define Subscribe(mgr, topic, callback) mgr.SetCallback(topic, \
                                                        std::bind(callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), \
                                                        this);

class C
{
};

class B
{
public:

    static B &getInstance()
    {
        static B instance;
        return instance;
    }

    void SetCallback(int iTopic, fpi fptr, C *p)
    {
        string ClassName = typeid(*p).name();
        printf("Classname:[%s]\n", ClassName.c_str());
        fptr(iTopic, (unsigned char *) "", 0);
    }

private:

    B()
    {

    }

    ~B()
    {

    }

    B(const B &);

    B &operator=(const B &);
};

class A : public C
{
public:
    int i = 1;

    void init()
    {
        B &b = B::getInstance();
        Subscribe(b, 1, &A::Callback1);
        Subscribe(b, 2, &A::Callback2);
    }

    void Callback1(int iTopic, const unsigned char *pData, size_t nSize)
    {
        printf("A::callback1 topic:%d i:%d\n", iTopic, i);
    }

    void Callback2(int iTopic, const unsigned char *pData, size_t nSize)
    {
        printf("A::callback2 topic:%d i:%d\n", iTopic, i);
    }
};

int main()
{
    A *a = new A();
    a->init();
}
