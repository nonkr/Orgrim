#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include <cstdio>
#include <cstdlib>
#include "MemoryChunk.h"

static const size_t MAX_SIZE = 1024;
static const size_t MIN_SIZE = sizeof(CMemoryChunk *);

struct HeapHeader_t
{
    size_t nSize;
};
struct MemoryHeap_t
{
    HeapHeader_t stHeader;
    char         pBuffer[];
};

class CMemoryPool
{
public:
    CMemoryPool(size_t nMaxSize, int iChunkBlockNum);

    ~CMemoryPool();

    void *New(size_t nSize);

    void Delete(void *pMem);

protected:
    void *LargeMalloc(size_t nSize);

    bool LargeFree(void *pMem);

private:
    CMemoryChunk **m_pChunkList;
    int          m_iChunkCount;
    size_t       m_nMaxSize;
    int          m_iChunkBlockNum;
};

//#define NEW(size_t size) CMemoryPool::New(size_t size){}


#endif


