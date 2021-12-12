#include <cstdio>
#include <cstdlib>
#include <cstring>
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

    cJSON *pDup = cJSON_Duplicate(pOneItem, true);

    cJSON_Delete(pJSON);

    cJSON *pNewObj = cJSON_CreateObject();
    // 将pDup加入到pNewObj后，在cJSON_Delete(pNewObj)时，会将pDup的内存也释放掉
    cJSON_AddItemToObject(pNewObj, "swpTempArea", pDup);
    // 所以如果后面还需要再复用pDup，需要再cJSON_Duplicate自身一遍
    pDup = cJSON_Duplicate(pDup, true);
    cJSON_Delete(pNewObj);

    cJSON *pNewObj2 = cJSON_CreateObject();
    cJSON_AddItemToObject(pNewObj2, "swpTempArea", pDup);
    cJSON_Delete(pNewObj2);
    return 0;
}
