#include <cstdlib>
#include "MemoryPool.h"

CMemoryPool::CMemoryPool(size_t nMaxSize, int iChunkBlockNum)
{
    m_nMaxSize       = nMaxSize;
    m_iChunkBlockNum = iChunkBlockNum;
    m_iChunkCount    = 0;

    if (m_nMaxSize > MAX_SIZE)
    {
        m_nMaxSize = MAX_SIZE;
    }

    for (size_t size = MIN_SIZE; size <= m_nMaxSize; size *= 2)
    {
        m_iChunkCount++;
    }

    m_pChunkList = new CMemoryChunk *[m_iChunkCount];

    int         i    = 0;
    for (size_t size = MIN_SIZE; size <= m_nMaxSize; size *= 2)
    {
        m_pChunkList[i++] = new CMemoryChunk(size, m_iChunkBlockNum);
    }
}

CMemoryPool::~CMemoryPool()
{
    for (int i = 0; i < m_iChunkCount; i++)
    {
        delete m_pChunkList[i];
    }
    delete[] m_pChunkList;
}

void *CMemoryPool::New(size_t nSize)
{
    int i = 0;

    if (nSize > m_nMaxSize)
    {
        return LargeMalloc(nSize);
    }

    for (size_t size = MIN_SIZE; size <= m_nMaxSize; size *= 2)
    {
        if (size >= nSize)
        {
            break;
        }
        i++;
    }

    return m_pChunkList[i]->Malloc();
}

void CMemoryPool::Delete(void *pMem)
{
    if (!LargeFree(pMem))
    {
        CMemoryChunk::Free(pMem);
    }
}

void *CMemoryPool::LargeMalloc(size_t nSize)
{
    MemoryHeap_t *pHeap = (MemoryHeap_t *) malloc(sizeof(MemoryHeap_t) + nSize);
    if (pHeap)
    {
        pHeap->stHeader.nSize = nSize;
        return pHeap->pBuffer;
    }
    return nullptr;
}

bool CMemoryPool::LargeFree(void *pMem)
{
    MemoryHeap_t *pHeap = (MemoryHeap_t *) ((char *) pMem - sizeof(HeapHeader_t));
    if (pHeap->stHeader.nSize > m_nMaxSize)
    {
        free(pHeap);
        return true;
    }
    return false;
}

