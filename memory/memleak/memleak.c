#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    char *p = NULL;
    size_t s;
    size_t total = 0;

    while (1)
    {
        s = 1 * 1024 * 1024;
        p = (char *) malloc(s);
        memset(p, 0x00, s);
        total += s;
        printf("total:%zu kB\n", total / 1024);
        sleep(3);
    }
    return 0;
}
