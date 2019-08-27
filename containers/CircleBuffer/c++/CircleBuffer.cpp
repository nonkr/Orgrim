//
// Created by songbinbin on 2019/8/21.
//

#include <cstdlib>
#include <cstring>
#include "CircleBuffer.h"
#include "PrintUtil.h"

CircleBuffer::CircleBuffer()
{
    m_pMsgDataPool = new MsgDataPool;
    memset(m_pMsgDataPool, 0x00, sizeof(MsgDataPool));

    for (int i = 0; i < MSG_POOL_SIZE; i++)
    {
        MsgData *pMsgData = new MsgData;
        memset(pMsgData, 0x00, sizeof(MsgData));
        m_pMsgDataPool->arrMsgData.at(i) = pMsgData;
    }
}

CircleBuffer::~CircleBuffer()
{
    // Delete all data
    m_pMsgDataPool->PoolCond.lock();

    for (int i = 0; i < MSG_POOL_SIZE; i++)
    {
        delete[] m_pMsgDataPool->arrMsgData.at(i)->pData;
        m_pMsgDataPool->arrMsgData.at(i)->bRelease = true;
        m_pMsgDataPool->arrMsgData.at(i)->DataLocker.broadcast();
        delete m_pMsgDataPool->arrMsgData.at(i);
    }

    m_pMsgDataPool->bRelease = true;
    m_pMsgDataPool->PoolCond.broadcast();
    m_pMsgDataPool->PoolCond.unlock();

    delete m_pMsgDataPool;
}

/**
 *  @fn	     static inline int SetFirstEmptyField(bitset<_N> &bs)
 *	@brief	 To set the first empty field in a bitset variable
 *	@param   bs        - The bitset variable to set
 *	@return	 int
 *  @note
 */
template<size_t _N>
inline int CircleBuffer::SetFirstEmptyField(bitset<_N> &bs)
{
    unsigned int uiSize = bs.size();

    for (unsigned int i = 0; i < uiSize; i++)
    {
        if (!bs.test(i))
        {
            bs.set(i);
            return i;
        }
    }
    return -1;
}

int CircleBuffer::Sub()
{
    FR_PRINT_GREEN("CircleBuffer::Sub\n");
    m_pMsgDataPool->PoolCond.lock();
    int iBitsetPos = CircleBuffer::SetFirstEmptyField(m_pMsgDataPool->bsSubscribers);
    if (iBitsetPos < 0)
    {
        FR_PRINT_RED("Can not add subscribers, is full\n");
        m_pMsgDataPool->PoolCond.unlock();
        return -1;
    }

    MsgData      *pMsgData     = nullptr;
    unsigned int uiEndPosition = 0;

    for (unsigned int i = m_pMsgDataPool->uiCurrWritePos; i < m_pMsgDataPool->uiCurrMsgDataNum; i++)
    {
        uiEndPosition = i;
        if (uiEndPosition >= MSG_POOL_SIZE)
        {
            uiEndPosition -= MSG_POOL_SIZE;
        }

        pMsgData = m_pMsgDataPool->arrMsgData.at(uiEndPosition);
        pMsgData->DataLocker.lock();
        FR_PRINT_GREEN("MsgData[%d] set bs %d\n", uiEndPosition, iBitsetPos);
        pMsgData->bs.set(iBitsetPos);
        pMsgData->DataLocker.unlock();
    }

    m_pMsgDataPool->arrSubPos[iBitsetPos] = m_pMsgDataPool->uiCurrWritePos;
    FR_PRINT_GREEN("CircleBuffer::Sub done, arrSubPos[%d]=%d\n", iBitsetPos, m_pMsgDataPool->uiCurrWritePos);

    m_pMsgDataPool->PoolCond.unlock();
    return iBitsetPos;
}

int CircleBuffer::UnSub(int iBitsetPos)
{
    m_pMsgDataPool->PoolCond.lock();
    m_pMsgDataPool->bsSubscribers.reset(iBitsetPos);

    MsgData *pMsgData = nullptr;

    for (int i = 0; i < MSG_POOL_SIZE; i++)
    {
        pMsgData = m_pMsgDataPool->arrMsgData.at(i);
        pMsgData->DataLocker.lock();
        pMsgData->bs.reset(iBitsetPos);
        pMsgData->DataLocker.unlock();
    }

    m_pMsgDataPool->PoolCond.unlock();

    return 0;
}

int CircleBuffer::Write(const char *pData, size_t nSize)
{
    FR_PRINT_YELLOW("\n==========\nWrite data\n");
    if (pData == nullptr || nSize <= 0)
    {
        FR_PRINT_RED("Push null data\n");
        return 1;
    }

    MsgData *pMsgData = nullptr;

    m_pMsgDataPool->PoolCond.lock();
    if (m_pMsgDataPool->bsSubscribers.none())
    {
        FR_PRINT_RED("No Subscribers, drop the data...\n");
        m_pMsgDataPool->PoolCond.unlock();
        return 1;
    }

    unsigned int uiEndPosition = m_pMsgDataPool->uiCurrMsgDataNum + m_pMsgDataPool->uiCurrWritePos;
    if (uiEndPosition >= MSG_POOL_SIZE)
    {
        uiEndPosition -= MSG_POOL_SIZE;
    }

    FR_PRINT_BLUE("add the %uth data\n", uiEndPosition);

    pMsgData = m_pMsgDataPool->arrMsgData.at(uiEndPosition);
    pMsgData->DataLocker.lock();
//        printf("delete pMsgData->pData:%p pMsgData->nSize:%zu\n", pMsgData->pData, pMsgData->nSize);
    delete[] pMsgData->pData;
    pMsgData->pData = new char[nSize + 1];
    memset(pMsgData->pData, 0x00, nSize + 1);
    FR_PRINT_PINK("add data:[%s]\n", pData);
    memcpy(pMsgData->pData, pData, nSize);
    pMsgData->nSize = nSize;
    pMsgData->bs    = m_pMsgDataPool->bsSubscribers;
    FR_PRINT_MAGENTA("add data, index:[%d] pMsgData->bs:[%s] pMsgData->pData:[%p:%s] pMsgData->nSize:[%zd]\n",
                     uiEndPosition,
                     pMsgData->bs.to_string().c_str(),
                     pMsgData->pData,
                     pMsgData->pData,
                     pMsgData->nSize);
    pMsgData->DataLocker.broadcast();
    pMsgData->DataLocker.unlock();

    m_pMsgDataPool->arrMsgData.at(uiEndPosition) = pMsgData;
    if (m_pMsgDataPool->uiCurrMsgDataNum == MSG_POOL_SIZE)
    {
        FR_PRINT_RED("Circle Buffer is full now, drop the %uth data\n", uiEndPosition);
    }
    else
    {
        m_pMsgDataPool->uiCurrMsgDataNum++;
    }
    m_pMsgDataPool->PoolCond.broadcast();
    FR_PRINT_BLUE("Published MsgDataPool->uiCurrWritePos: %d\n", m_pMsgDataPool->uiCurrWritePos);
    FR_PRINT_BLUE("Published MsgDataPool->uiCurrMsgDataNum: %d\n", m_pMsgDataPool->uiCurrMsgDataNum);
//        SubscriberSet::PrintMsgDataPool(m_pMsgDataPool);
    m_pMsgDataPool->PoolCond.unlock();

    FR_PRINT_BLUE("SubscriberSet::Publish done...\n");
    return 0;
}

int CircleBuffer::Read(int iBitsetPos, char *pData, size_t *pSize)
{
    FR_PRINT_BLUE("CircleBuffer::Read\n");

    MsgData *pMsgData = nullptr;

    m_pMsgDataPool->PoolCond.lock();
    while (m_pMsgDataPool->uiCurrMsgDataNum <= 0 && !m_pMsgDataPool->bRelease)
    {
        FR_PRINT_BLUE("CircleBuffer::Read PoolCond.wait()\n");
        m_pMsgDataPool->PoolCond.wait();
        if (m_pMsgDataPool->bRelease)
        {
            m_pMsgDataPool->PoolCond.unlock();
            return 1;
        }
    }

    unsigned int uiCurrReadPos = m_pMsgDataPool->arrSubPos[iBitsetPos];
    pMsgData = m_pMsgDataPool->arrMsgData.at(uiCurrReadPos);
    m_pMsgDataPool->PoolCond.unlock();

    pMsgData->DataLocker.lock();
    while ((!pMsgData->bs.test(iBitsetPos) || pMsgData->pData == nullptr || pMsgData->nSize <= 0) &&
           !pMsgData->bRelease)
    {
        FR_PRINT_ORANGE("try to read data, index:[%d] pMsgData->bs:[%s] pMsgData->pData:[%p] pMsgData->nSize:[%zu]\n",
                        uiCurrReadPos, pMsgData->bs.to_string().c_str(), pMsgData->pData, pMsgData->nSize);
        FR_PRINT_BLUE("CircleBuffer::Read DataLocker.wait()\n");
        pMsgData->DataLocker.wait();
        if (pMsgData->bRelease)
        {
            pMsgData->DataLocker.unlock();
            return 1;
        }
    }

    FR_PRINT_YELLOW(
        "read data, bitPos:[%d] index:[%d] pMsgData->bs:[%s] pMsgData->pData:[%p:%s] pMsgData->nSize:[%zu]\n",
        iBitsetPos, uiCurrReadPos, pMsgData->bs.to_string().c_str(), pMsgData->pData, pMsgData->pData, pMsgData->nSize);
    memcpy(pData, pMsgData->pData, pMsgData->nSize);
    *pSize = pMsgData->nSize;
    pMsgData->bs.reset(iBitsetPos);
    if (pMsgData->bs.none())
    {
        m_pMsgDataPool->PoolCond.lock();
        m_pMsgDataPool->uiCurrMsgDataNum--;
        if (m_pMsgDataPool->uiCurrMsgDataNum < 0)
        {
            m_pMsgDataPool->uiCurrMsgDataNum = 0;
        }
        m_pMsgDataPool->uiCurrWritePos++;
        if (m_pMsgDataPool->uiCurrWritePos >= MSG_POOL_SIZE)
        {
            m_pMsgDataPool->uiCurrWritePos -= MSG_POOL_SIZE;
        }
        m_pMsgDataPool->PoolCond.unlock();
    }
    pMsgData->DataLocker.unlock();

    m_pMsgDataPool->PoolCond.lock();
    m_pMsgDataPool->arrSubPos[iBitsetPos]++;
    if (m_pMsgDataPool->arrSubPos[iBitsetPos] >= MSG_POOL_SIZE)
    {
        m_pMsgDataPool->arrSubPos[iBitsetPos] -= MSG_POOL_SIZE;
    }
    m_pMsgDataPool->PoolCond.unlock();

    FR_PRINT_BLUE("CircleBuffer::Read done\n");
    return 0;
}
