/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2019/7/1 18:50
 *
 */

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "cJSON.h"

using namespace std;

int main(int argc, char *argv[])
{
    ifstream fp("../test2.json", std::ifstream::ate);
    if (!fp.is_open())
    {
        printf("No WIFI station config: test2.json\n");
        return 1;
    }

    // read whole file to string
    std::string str;
    str.reserve(fp.tellg());
    fp.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(fp)), std::istreambuf_iterator<char>());
    fp.close();

    cJSON *pJSON = cJSON_Parse(str.c_str());
    if (!pJSON)
    {
        printf("Failed to cJSON_Parse test2.json\n");
        return 1;
    }

    char *pStr = cJSON_Print(pJSON);
    printf("%s\n", pStr);
    free(pStr);

    cJSON *pNewName = cJSON_CreateString("Lei");
    cJSON_ReplaceItemInObject(pJSON, "name", pNewName);

    pStr = cJSON_Print(pJSON);
    printf("%s\n", pStr);
    free(pStr);

    cJSON_Delete(pJSON);

    return 0;
}
