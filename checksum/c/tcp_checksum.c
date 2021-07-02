#include <stdio.h>
#include "test_values.h"

/**
 * 1、先将需要计算checksum数据中的checksum设为0；
 * 2、计算checksum的数据按2byte划分开来，每2byte组成一个16bit的值，如果最后有单个byte的数据，补一个byte的0组成2byte；
 * 3、将所有的16bit值累加到一个32bit的值中；
 * 4、将32bit值的高16bit与低16bit相加到一个新的32bit值中，若新的32bit值大于0Xffff，再将新值的高16bit与低16bit相加；
 * 5、将上一步计算所得的16bit值按位取反，即得到checksum值，存入数据的checksum字段即可
 */
static uint16_t chksum(void *dataptr, uint16_t len)
{
    uint32_t acc;
    uint16_t src;
    uint8_t  *octetptr;

    acc      = 0;
    octetptr = (uint8_t *) dataptr;
    while (len > 1)
    {
        src = (*octetptr) << 8;
        octetptr++;
        src |= (*octetptr);
        octetptr++;
        acc += src;
        len -= 2;
    }
    if (len > 0)
    {
        src = (*octetptr) << 8;
        acc += src;
    }

    acc = (acc >> 16) + (acc & 0x0000FFFFUL);
    if ((acc & 0xFFFF0000UL) != 0)
    {
        acc = (acc >> 16) + (acc & 0x0000FFFFUL);
    }

    src = (uint16_t) acc;
    return ~src;
}

int main()
{
    uint16_t checksum1 = chksum((void *) test_value1, sizeof(test_value1));
    uint16_t checksum2 = chksum((void *) test_value2, sizeof(test_value2));
    uint16_t checksum3 = chksum((void *) test_value3, sizeof(test_value3));
    uint16_t checksum4 = chksum((void *) test_value4, sizeof(test_value4));
    uint16_t checksum5 = chksum((void *) test_value5, sizeof(test_value5));
    uint16_t checksum6 = chksum((void *) test_value6, sizeof(test_value6));
    printf("checksum1:0x%02X\n", checksum1);
    printf("checksum2:0x%02X\n", checksum2);
    printf("checksum3:0x%02X\n", checksum3);
    printf("checksum4:0x%02X\n", checksum4);
    printf("checksum5:0x%02X\n", checksum5);
    printf("checksum6:0x%02X\n", checksum6);
    return 0;
}
