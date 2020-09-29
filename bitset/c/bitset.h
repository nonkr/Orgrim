#pragma once

#include <limits.h>
#include <memory.h>

#define BITMASK(b)          (1 << ((b) % CHAR_BIT))
#define BITSLOT(b)          ((b) / CHAR_BIT)
#define BITSET(a, b)        ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b)      ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b)       ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb)       ((nb + CHAR_BIT - 1) / CHAR_BIT)
#define BITLEN(a)           (sizeof(a) / sizeof(char))
#define BITCLEAR_ALL(arr)   memset(arr, 0, BITLEN(arr));
#define BITSET_ALL(arr)     memset(arr, UCHAR_MAX, BITLEN(arr));

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
                      ((byte) & 0x80 ? '1' : '0'), \
                      ((byte) & 0x40 ? '1' : '0'), \
                      ((byte) & 0x20 ? '1' : '0'), \
                      ((byte) & 0x10 ? '1' : '0'), \
                      ((byte) & 0x08 ? '1' : '0'), \
                      ((byte) & 0x04 ? '1' : '0'), \
                      ((byte) & 0x02 ? '1' : '0'), \
                      ((byte) & 0x01 ? '1' : '0')

#define SHORT_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN "" BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY_PRETTY_PATTERN BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define SHORT_TO_BINARY(s) BYTE_TO_BINARY((s) >> 8), BYTE_TO_BINARY(s)

#define INT_TO_BINARY_PATTERN SHORT_TO_BINARY_PATTERN "" SHORT_TO_BINARY_PATTERN
#define INT_TO_BINARY_PRETTY_PATTERN SHORT_TO_BINARY_PRETTY_PATTERN " " SHORT_TO_BINARY_PRETTY_PATTERN
#define INT_TO_BINARY(i) BYTE_TO_BINARY((i) >> 24), \
                         BYTE_TO_BINARY((i) >> 16), \
                         BYTE_TO_BINARY((i) >> 8), \
                         BYTE_TO_BINARY(i)

#define LONGLONG_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN "" INT_TO_BINARY_PATTERN
#define LONGLONG_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN " " INT_TO_BINARY_PRETTY_PATTERN
#define LONGLONG_TO_BINARY(ll) BYTE_TO_BINARY((ll) >> 56), \
                               BYTE_TO_BINARY((ll) >> 48), \
                               BYTE_TO_BINARY((ll) >> 40), \
                               BYTE_TO_BINARY((ll) >> 32), \
                               BYTE_TO_BINARY((ll) >> 24), \
                               BYTE_TO_BINARY((ll) >> 16), \
                               BYTE_TO_BINARY((ll) >> 8), \
                               BYTE_TO_BINARY(ll)

#define CHAR1_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN
#define CHAR1_TO_BINARY_PRETTY_PATTERN BYTE_TO_BINARY_PATTERN
#define CHAR1_TO_BINARY(arr) BYTE_TO_BINARY(arr[0])

#define CHAR2_TO_BINARY_PATTERN SHORT_TO_BINARY_PATTERN
#define CHAR2_TO_BINARY_PRETTY_PATTERN SHORT_TO_BINARY_PRETTY_PATTERN
#define CHAR2_TO_BINARY(arr) BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])

#define CHAR3_TO_BINARY_PATTERN SHORT_TO_BINARY_PATTERN "" BYTE_TO_BINARY_PATTERN
#define CHAR3_TO_BINARY_PRETTY_PATTERN SHORT_TO_BINARY_PRETTY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define CHAR3_TO_BINARY(arr) BYTE_TO_BINARY(arr[2]), \
                           BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])

#define CHAR4_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN
#define CHAR4_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN
#define CHAR4_TO_BINARY(arr) BYTE_TO_BINARY(arr[3]), \
                           BYTE_TO_BINARY(arr[2]), \
                           BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])

#define CHAR5_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN "" BYTE_TO_BINARY_PATTERN
#define CHAR5_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define CHAR5_TO_BINARY(arr) BYTE_TO_BINARY(arr[4]), \
                           BYTE_TO_BINARY(arr[3]), \
                           BYTE_TO_BINARY(arr[2]), \
                           BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])

#define CHAR6_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN "" SHORT_TO_BINARY_PATTERN
#define CHAR6_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN " " SHORT_TO_BINARY_PRETTY_PATTERN
#define CHAR6_TO_BINARY(arr) BYTE_TO_BINARY(arr[5]), \
                           BYTE_TO_BINARY(arr[4]), \
                           BYTE_TO_BINARY(arr[3]), \
                           BYTE_TO_BINARY(arr[2]), \
                           BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])

#define CHAR7_TO_BINARY_PATTERN INT_TO_BINARY_PATTERN "" SHORT_TO_BINARY_PATTERN "" BYTE_TO_BINARY_PATTERN
#define CHAR7_TO_BINARY_PRETTY_PATTERN INT_TO_BINARY_PRETTY_PATTERN " " SHORT_TO_BINARY_PRETTY_PATTERN " " BYTE_TO_BINARY_PATTERN
#define CHAR7_TO_BINARY(arr) BYTE_TO_BINARY(arr[6]), \
                           BYTE_TO_BINARY(arr[5]), \
                           BYTE_TO_BINARY(arr[4]), \
                           BYTE_TO_BINARY(arr[3]), \
                           BYTE_TO_BINARY(arr[2]), \
                           BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])

#define CHAR8_TO_BINARY_PATTERN LONGLONG_TO_BINARY_PATTERN
#define CHAR8_TO_BINARY_PRETTY_PATTERN LONGLONG_TO_BINARY_PRETTY_PATTERN
#define CHAR8_TO_BINARY(arr) BYTE_TO_BINARY(arr[7]), \
                           BYTE_TO_BINARY(arr[6]), \
                           BYTE_TO_BINARY(arr[5]), \
                           BYTE_TO_BINARY(arr[4]), \
                           BYTE_TO_BINARY(arr[3]), \
                           BYTE_TO_BINARY(arr[2]), \
                           BYTE_TO_BINARY(arr[1]), \
                           BYTE_TO_BINARY(arr[0])
