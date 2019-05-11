/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/5/11 16:00
 *
 */

#include <cstdio>
#include <cstdlib>
#include "cJSON.h"

int main(int argc, char *argv[])
{
    cJSON *pJSON = cJSON_CreateArray();
    cJSON *pOneItem = cJSON_CreateObject();
    cJSON_AddStringToObject(pOneItem, "name", "John");
    cJSON_AddNumberToObject(pOneItem, "age", 16);
    cJSON_AddItemToArray(pJSON, pOneItem);

    char *pStr = cJSON_Print(pJSON);
    if (pStr)
    {
        printf("%s\n", pStr);
        free(pStr);
    }

    cJSON_Delete(pJSON);

    return 0;
}
