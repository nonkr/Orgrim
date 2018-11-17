#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

int main()
{
    uint64_t a = 0x12;

    printf("a:%"PRIu64"\n", a);
    return 0;
}
