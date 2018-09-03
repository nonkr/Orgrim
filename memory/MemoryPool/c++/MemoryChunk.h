#ifndef _MEMCHUNK_H_
#define _MEMCHUNK_H_

#include <pthread.h>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include "Locker.h"

class CMemoryChunk;

struct BlockHeader_t
{
    CMemoryChunk *pChunk;
    size_t       nSize;
};

struct MemBlock_t;
struct BlockData_t
{
    union
    {
        MemBlock_t *pNext;
        char       data_ptr;
    };
};
struct MemBlock_t
{
    BlockHeader_t stHeader;
    BlockData_t   stData;
};

class CMemoryChunk
{
public:
    CMemoryChunk(size_t nSize, int iBlockNum);

    ~CMemoryChunk();

    void *Malloc();

    static void Free(void *pMem = nullptr);

    void Free(MemBlock_t *pBlock);

protected:
    MemBlock_t *CreateBlock();

    void DeleteBlock(MemBlock_t *pBlock);

private:
    locker     m_locker;
//    SpinLocker m_locker;
    MemBlock_t *m_pFreeBlock;   // idle block
    size_t     m_nSize;         // block size
    int        m_iBlockCount;   // block num
};

#endif

