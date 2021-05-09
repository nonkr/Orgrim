#include <stdio.h>
#include <string.h>

int main()
{
    char filename[128] = {0};
    snprintf(filename, sizeof(filename), "abc.log");
    printf("[%s] len:%zu\n", filename, strlen(filename));

    snprintf(filename, sizeof(filename), "abcd.log");
    printf("[%s] len:%zu\n", filename, strlen(filename));

    snprintf(filename, sizeof(filename), "ab.log");
    printf("[%s] len:%zu\n", filename, strlen(filename));

    snprintf(filename, sizeof(filename), "a");
    printf("[%s] len:%zu\n", filename, strlen(filename));
    return 0;
}
