/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under GPL, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/25 16:26
 *
 */

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using std::vector;

typedef enum
{
    NODE_TYPE_FIXED = 10,
    NODE_TYPE_HASHED,
    NODE_TYPE_RANGE,
    NODE_TYPE_UNKNOWN,
} NodeValueType;

typedef struct
{
    char cMin;
    char cMax;
} NodeRangeValue;

typedef struct
{
    NodeValueType eValueType;
    union
    {
        char           cValue;
        vector<char>   *pvValue;
        NodeRangeValue *stRangeValue;
    }             Value;
} NodeValue;

int main()
{
    vector<NodeValue *> nodes;

    // set
    auto *pNodeValue = new NodeValue;
    pNodeValue->eValueType = NODE_TYPE_HASHED;
    auto *pvValue = new vector<char>();
    pvValue->push_back('A');
    pvValue->push_back('B');
    pNodeValue->Value.pvValue = pvValue;
    nodes.push_back(pNodeValue);

    auto *pNodeValue2 = new NodeValue;
    pNodeValue2->eValueType   = NODE_TYPE_FIXED;
    pNodeValue2->Value.cValue = 'Y';
    nodes.push_back(pNodeValue2);

    auto *pNodeValue3 = new NodeValue;
    pNodeValue3->eValueType = NODE_TYPE_RANGE;
    auto *pRangeValue = new NodeRangeValue;
    pRangeValue->cMin = 2;
    pRangeValue->cMax = 5;
    pNodeValue3->Value.stRangeValue = pRangeValue;
    nodes.push_back(pNodeValue3);

    // get
    printf("size:[%zd]\n", nodes.size());
    for (auto n : nodes)
    {
        printf("ValueType:[%d]\n", n->eValueType);
        switch (n->eValueType)
        {
            case NODE_TYPE_FIXED:
                printf("Value:[%c]\n", n->Value.cValue);
                break;
            case NODE_TYPE_HASHED:
                for (auto v : *(n->Value.pvValue))
                {
                    printf("Value:[%c]\n", v);
                }
                break;
            case NODE_TYPE_RANGE:
                printf("Value:[%d...%d]\n", n->Value.stRangeValue->cMin, n->Value.stRangeValue->cMax);
                break;
        }
    }

    // delete
    for (auto n : nodes)
    {
        switch (n->eValueType)
        {
            case NODE_TYPE_HASHED:
                delete n->Value.pvValue;
                break;
            case NODE_TYPE_RANGE:
                delete n->Value.stRangeValue;
                break;
        }
        delete n;
    }

    return 0;
}