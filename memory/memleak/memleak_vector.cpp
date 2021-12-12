#include <memory.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <vector>

struct People
{
    int a;
    char arr[32];
};

int main()
{
    std::vector<People> v;
    People p;

    while (true)
    {
        for (int i = 0; i < 100000; i++)
        {
            p.a = i;
            v.emplace_back(p);
        }
        printf("size:%zu capacity:%zu\n", v.size(), v.capacity());
//        std::vector<People>().swap(v);
        v.clear();
        printf("size:%zu capacity:%zu after clear\n", v.size(), v.capacity());

        for (int i = 0; i < 300000; i++)
        {
            p.a = i;
            v.emplace_back(p);
        }
        printf("size:%zu capacity:%zu\n", v.size(), v.capacity());
//        std::vector<int>().swap(v);
        v.clear();
        printf("size:%zu capacity:%zu after clear\n", v.size(), v.capacity());

        sleep(1);
    }
    return 0;
}
