//
// Created by songbinbin on 2020/5/22.
//

#include "PrintUtil.h"

int main()
{
    int count = 0;
    while (true)
    {
        FR_PRINT_ORANGE("a", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:%d\n", count++);
        usleep(100000);
    }
    return 0;
}
