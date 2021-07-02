#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int a = 1;
int b = 10;

int main()
{
    srand(time(NULL));
    while (1)
    {
        printf("===================\n");
        printf("[%d, %d] %d\n", a, b, rand() % (b - a + 1) + a);        //[a,b] 包括a和b
    }

    return 0;
}
