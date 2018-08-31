/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/8/30 15:15
 *
 *
 * NOTE: 本例亦适用于C++11之前的多线程环境。
 * 这里用到了local static变量，dummy_的作用是即使在main函数之前没有调用getInstance，它依然会作为最后一道屏障保证在进入main函数
 * 之前构造完成Singleton对象。这样就避免了在进入main函数后的多线程环境中初始化的各种问题了。
 * 但是此种方法只能在main函数执行之前的环境是单线程的环境下才能正确工作。
 */

#include <cstdio>

class LocalStaticSingleton
{
public:
    static LocalStaticSingleton &getInstance()
    {
        printf("getInstance\n");
        static Creater creater;
        return creater.getValue();
    }

private:
    LocalStaticSingleton()
    {
        printf("c'tor\n");
    };

    ~LocalStaticSingleton()
    {
        printf("d'tor\n");
    };

    LocalStaticSingleton(const LocalStaticSingleton &);

    LocalStaticSingleton &operator=(const LocalStaticSingleton &);

    class Creater
    {
    public:
        Creater()
            : _value(new LocalStaticSingleton())
        {
        }

        ~Creater()
        {
            delete _value;
            _value = nullptr;
        }

        LocalStaticSingleton &getValue()
        {
            return *_value;
        }

        LocalStaticSingleton *_value;
    };

    class Dummy
    {
    public:
        Dummy()
        {
            printf("Dummy\n");
            LocalStaticSingleton::getInstance();
        }
    };

    static Dummy _dummy;
};

LocalStaticSingleton::Dummy LocalStaticSingleton::_dummy;

int main()
{
    printf("main\n");
    LocalStaticSingleton &s1 = LocalStaticSingleton::getInstance();
    LocalStaticSingleton &s2 = LocalStaticSingleton::getInstance();

    printf("s1:%p\n", &s1);
    printf("s2:%p\n", &s2);

    return 0;
}
