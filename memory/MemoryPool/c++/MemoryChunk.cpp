#include "MemoryChunk.h"

CMemoryChunk::CMemoryChunk(size_t nSize, int iBlockNum)
{
    m_pFreeBlock  = nullptr;
    m_nSize       = nSize;
    m_iBlockCount = 0;
    MemBlock_t *pBlock;

    while (iBlockNum--)
    {
        pBlock = CreateBlock();
        if (!pBlock)
        {
            break;
        }
        pBlock->stData.pNext = m_pFreeBlock;
        m_pFreeBlock = pBlock;
    }
}

CMemoryChunk::~CMemoryChunk()
{
    int        iFreedCount = 0;
    MemBlock_t *pBlock     = m_pFreeBlock;

    while (pBlock)
    {
        m_pFreeBlock = pBlock->stData.pNext;
        DeleteBlock(pBlock);
        pBlock = m_pFreeBlock;
        iFreedCount++;
    }

    assert(iFreedCount == m_iBlockCount); // Make sure the release is complete
}

void *CMemoryChunk::Malloc()
{
    MemBlock_t *pBlock;

    m_locker.lock();

    if (m_pFreeBlock)
    {
        pBlock       = m_pFreeBlock;
        m_pFreeBlock = pBlock->stData.pNext;
    }
    else
    {
        if (!(pBlock = CreateBlock()))
        {
            m_locker.unlock();
            return nullptr;
        }
    }

    m_locker.unlock();

    return &pBlock->stData.data_ptr;
}

void CMemoryChunk::Free(void *pMem)
{
    MemBlock_t *pBlock = (MemBlock_t *) ((char *) pMem - sizeof(BlockHeader_t));
    pBlock->stHeader.pChunk->Free(pBlock);
}

void CMemoryChunk::Free(MemBlock_t *pBlock)
{
    m_locker.lock();
    pBlock->stData.pNext = m_pFreeBlock;
    m_pFreeBlock = pBlock;
    m_locker.unlock();
}

MemBlock_t *CMemoryChunk::CreateBlock()
{
    MemBlock_t *pBlock = (MemBlock_t *) malloc(sizeof(BlockHeader_t) + m_nSize);

    if (pBlock)
    {
        pBlock->stHeader.pChunk = this;
        pBlock->stHeader.nSize  = m_nSize;
        m_iBlockCount++;
    }

    return pBlock;
}

void CMemoryChunk::DeleteBlock(MemBlock_t *pBlock)
{
    free(pBlock);
}
