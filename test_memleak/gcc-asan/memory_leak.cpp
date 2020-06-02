#include <cstdlib>

void func1() { malloc(7); }

void func2() { malloc(5); }

int main()
{
    func1();
    func2();
    return 0;
}
