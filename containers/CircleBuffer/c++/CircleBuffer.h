//
// Created by songbinbin on 2019/8/21.
//

#ifndef __CIRCLEBUFFER_H__
#define __CIRCLEBUFFER_H__

#include <bitset>
#include <array>
#include "Locker.h"

#define MSG_POOL_SIZE  (8)
#define MSG_SUB_SIZE   (8)

using std::array;
using std::bitset;

/**
 * @brief  To store one message, include a locker inside
 */
struct MsgData
{
    char                 *pData   = nullptr;
    size_t               nSize    = 0;
    bitset<MSG_SUB_SIZE> bs;
    cond                 DataLocker;
    bool                 bRelease = false;
};

/**
 * @brief  The pool to store each message, and size is specificed
 */
struct MsgDataPool
{
    cond                            PoolCond;
    bitset<MSG_SUB_SIZE>            bsSubscribers;
    array<MsgData *, MSG_POOL_SIZE> arrMsgData;
    unsigned int                    arrSubPos[MSG_SUB_SIZE];
    // The bitset variable for recording the subscriber number and postion
    unsigned int                    uiCurrWritePos   = 0;
    unsigned int                    uiCurrMsgDataNum = 0;
    bool                            bRelease         = false;
};

class CircleBuffer
{
public:
    CircleBuffer();

    ~CircleBuffer();

    int Sub();

    int UnSub(int iBitsetPos);

    int Write(const char *pData, size_t nSize);

    int Read(int iBitsetPos, char *pData, size_t *pSize);

private:

    // A pool to store each message data
    MsgDataPool *m_pMsgDataPool;

    /**
     *  @fn	     static inline int SetFirstEmptyField(bitset<_N> &bs)
     *	@brief	 To set the first empty field in a bitset variable
     *	@param   bs        - The bitset variable to set
     *	@return	 int
     *  @note
     */
    template<size_t _N>
    static inline int SetFirstEmptyField(bitset<_N> &bs);
};

#endif //__CIRCLEBUFFER_H__
