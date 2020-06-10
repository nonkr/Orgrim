#include <stdio.h>
#include "test_values.h"

int main()
{
    uint8_t checksum1 = 0;
    uint8_t checksum2 = 0;
    uint8_t checksum3 = 0;
    uint8_t checksum4 = 0;
    uint8_t checksum5 = 0;
    uint8_t checksum6 = 0;
    size_t  s1        = sizeof(test_value1);
    size_t  s2        = sizeof(test_value2);
    size_t  s3        = sizeof(test_value3);
    size_t  s4        = sizeof(test_value4);
    size_t  s5        = sizeof(test_value5);
    size_t  s6        = sizeof(test_value6);

    for (size_t i = 0; i < s1; i++)
    {
        checksum1 += test_value1[i];
    }
    for (size_t i = 0; i < s2; i++)
    {
        checksum2 += test_value2[i];
    }
    for (size_t i = 0; i < s3; i++)
    {
        checksum3 += test_value3[i];
    }
    for (size_t i = 0; i < s4; i++)
    {
        checksum4 += test_value4[i];
    }
    for (size_t i = 0; i < s5; i++)
    {
        checksum5 += test_value5[i];
    }
    for (size_t i = 0; i < s6; i++)
    {
        checksum6 += test_value6[i];
    }
    printf("checksum1:0x%02X\n", checksum1);
    printf("checksum2:0x%02X\n", checksum2);
    printf("checksum3:0x%02X\n", checksum3);
    printf("checksum4:0x%02X\n", checksum4);
    printf("checksum5:0x%02X\n", checksum5);
    printf("checksum6:0x%02X\n", checksum6);
    return 0;
}
