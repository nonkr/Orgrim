//
// Created by songbinbin on 2020/5/8.
//

#ifndef __ORGRIM_PACK_H__
#define __ORGRIM_PACK_H__

#pragma pack(push, 1)

struct PeoplePacked
{
    char      A;
    short     B;
    int       C;
    long long D;
};

#pragma pack(pop)

struct PeopleUnPacked
{
    char      A;
    short     B;
    int       C;
    long long D;
};

#endif //__ORGRIM_PACK_H__
