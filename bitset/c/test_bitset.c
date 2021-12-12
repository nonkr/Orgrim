#include <stdint.h>
#include <stdio.h>
#include "bitset.h"

int main()
{
//    uint64_t a = 0b0011000000001111;
//    printf("a:["LONGLONG_TO_BINARY_PRETTY_PATTERN"]\n", LONGLONG_TO_BINARY(a));
//    BITCLEAR(a, 1);
//    printf("a:["LONGLONG_TO_BINARY_PRETTY_PATTERN"]\n", LONGLONG_TO_BINARY(a));

#define TEST_MAX_COUNT (10)
    char arr1[BITNSLOTS(TEST_MAX_COUNT)] = {0};
    printf("BITNSLOTS(%d) = %d\n", TEST_MAX_COUNT, BITNSLOTS(TEST_MAX_COUNT));

    printf("arr1 init:["CHAR2_TO_BINARY_PRETTY_PATTERN"]\n", CHAR2_TO_BINARY(arr1));
    BITSET_ALL(arr1, TEST_MAX_COUNT)
    printf("arr1 set all:["CHAR2_TO_BINARY_PRETTY_PATTERN"]\n", CHAR2_TO_BINARY(arr1));
    BITCLEAR_ALL(arr1)
    printf("arr1 clear all:["CHAR2_TO_BINARY_PRETTY_PATTERN"]\n", CHAR2_TO_BINARY(arr1));

//    char arr[BITNSLOTS(64)] = {0};
//    BITSET(arr, 0);
//    BITSET(arr, 63);
//    printf("arr:["CHAR8_TO_BINARY_PRETTY_PATTERN"]\n", CHAR8_TO_BINARY(arr));
//    printf("%ld\n", BITLEN(arr));
    return 0;
}
