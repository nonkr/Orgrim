#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include "bitset.h"

static uint64_t BinStringToU64(const char *str)
{
    uint64_t value = 0;
    if (str == NULL) return value;
    if (strncasecmp(str, "0B", 2) != 0) return value;
    size_t len = strlen(str);
    if (len <= 2 || len > 66) return value;

    for (size_t i = 2; i < len; i++)
    {
        char c      = str[i];
        int  offset = len - i - 1;
        if (c == '1')
        {
            value |= (1 << offset);
        }
        else if (c == '0')
        {
        }
        else
        {
            return 0;
        }
    }

    return value;
}

int main()
{
    printf("%lu\n", BinStringToU64((char *) "0B1010110"));
    return 0;
}
