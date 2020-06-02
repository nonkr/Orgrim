#include <iostream>

int main()
{
    int *array = new int[100];
    delete[] array;
    int a = array[0];  // error
    return 0;
}
