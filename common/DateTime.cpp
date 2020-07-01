
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <chrono>

#ifndef _WINDOWS
#include <sys/time.h>
#include <memory.h>
#endif

#include <time.h>
#include <assert.h>
#include "DateTime.h"
using namespace std;


#ifdef _WINDOWS
LARGE_INTEGER mCpuFrequency = { 0 };
#endif

TimeTracer *TimeTracer::mInstance[MAX_TIME_TRACE_INSTANCE];


TimeInterval::TimeInterval()
{
    mMaxInterval = mMinInterval = mLastInterval;
    for (int i = 0; i < MAX_AVERAGE_COUNT; i++)
        mLastIntervals[i] = 0;
    mIndex = 0;
    mMaxInterval = 1000 * 1000 * 1000;
    mMinInterval = 0;
    mLastInterval = Tic();
}

TimeInterval::~TimeInterval()
{
}

uint64_t TimeInterval::Tic()
{
    mLastTime = GetCurrentTick();
    return mLastTime;
}

uint64_t TimeInterval::Tac()
{
    return Update();
}

uint64_t TimeInterval::Update()
{
    uint64_t ct = GetCurrentTick();
    mLastInterval = ct - mLastTime;
    mLastTime = ct;
    mLastIntervals[mIndex++] = mLastInterval;
	if (mIndex >= MAX_AVERAGE_COUNT)
	{
        mIndex = 0;
	}

	if (mLastInterval > mMaxInterval)
	{
        mMaxInterval = mLastInterval;
	}

	if (mLastInterval < mMinInterval)
	{
        mMinInterval = mLastInterval;
	}

    return mLastInterval;
}

uint64_t TimeInterval::Update(uint64_t last_time)
{
    uint64_t ct = GetCurrentTick();
    mLastTime = last_time;
    mLastInterval = ct - mLastTime;
    mLastTime = ct;
    mLastIntervals[mIndex++] = mLastInterval;
	if (mIndex >= MAX_AVERAGE_COUNT)
	{
        mIndex = 0;
	}

	if (mLastInterval > mMaxInterval)
	{
        mMaxInterval = mLastInterval;
	}

	if (mLastInterval < mMinInterval)
	{
        mMinInterval = mLastInterval;
	}

    return mLastInterval;
}

uint64_t TimeInterval::GetMaxInterval()
{
    return mMaxInterval;
}

uint64_t TimeInterval::GetMinInterval()
{
    return mMinInterval;
}

uint64_t TimeInterval::GetLastInterval()
{
    return mLastInterval;
}

uint64_t TimeInterval::GetAverageInterval()
{
    uint64_t sum = 0;
	for (int i = 0; i < MAX_AVERAGE_COUNT; i++)
	{
        sum += mLastIntervals[i];
	}

    return sum / MAX_AVERAGE_COUNT;
}

TimeTracer::~TimeTracer()
{
    if (mTimeTraceItems) {
        delete[] mTimeTraceItems;
        mTimeTraceItems = NULL;
    }
}

TimeTracer * TimeTracer::Instance(int32_t id)
{
    if (id >= MAX_TIME_TRACE_INSTANCE)
        id = 0;

    if (mInstance[id] == NULL) {
        mInstance[id] = new TimeTracer(id);
		assert(mInstance != nullptr);
    }

    return mInstance[id];
}

void TimeTracer::Start()
{
    mTimeTraceItemIndex = 0;
    mStartTime = GetCurrentTick();
}

void TimeTracer::Stop()
{
    char *pn;
    TimeTraceItem *p;
    int n = mTimeTraceItemIndex;
    if (n <= 0)	return;

    p = &mTimeTraceItems[0];
    pn = ((pn = strrchr(p->file_name, '/')) || (pn = strrchr(p->file_name, '\\'))) ? pn + 1 : p->file_name;

    for (int i = 1; i < n; i++) {
        p = &mTimeTraceItems[i];
        pn = ((pn = strrchr(p->file_name, '/')) || (pn = strrchr(p->file_name, '\\'))) ? pn + 1 : p->file_name;
    }
}

void TimeTracer::AddTracePoint(const char * file, const char * function, int32_t line)
{
    if (mTimeTraceItemIndex >= MAX_TIME_TRACE_ITEMS)
        return;

    TimeTraceItem *tti = &mTimeTraceItems[mTimeTraceItemIndex++];

    strcpy(tti->file_name, file);
    strcpy(tti->function_name, function);
    tti->line = line;
    tti->time_stamp = GetCurrentTick();
}

TimeTracer::TimeTracer(int32_t id)
{
    mTimeTraceItems = new TimeTraceItem[MAX_TIME_TRACE_ITEMS];
	assert(mTimeTraceItems != nullptr);

    mTimeTraceItemIndex = 0;
    mInstanceId = id;
    mStartTime = GetCurrentTick();
}

//Seconds since 1970-1-1 00:00:00
uint64_t GetSystemTickSec()
{
    auto now = std::chrono::system_clock::now();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return sec;
}

//Milli seconds since 1970-1-1 00:00:00
uint64_t GetSystemTickMs()
{
    auto now = std::chrono::system_clock::now();
    auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return milli;
}

//Micro seconds since 1970-1-1 00:00:00
uint64_t GetSystemTick()
{
    auto now = std::chrono::system_clock::now();
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return micro;
}

//std::chrono::steady_clock 为了表示稳定的时间间隔，后一次调用now()得到的时间总是比前一次的值大（这句话的意思其实是，如果中途修改了系统时间，也不影响now()的结果），每次tick都保证过了稳定的时间间隔。
uint64_t GetCurrentTick()
{
    auto tp = std::chrono::steady_clock::now();
    auto cur_time = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
    return cur_time;
}

uint32_t GetCurrentTickSec()
{
    auto tp = std::chrono::steady_clock::now();
    time_t cur_time = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
    return cur_time;
}

uint64_t GetCurrentTickMs()
{
    auto tp = std::chrono::steady_clock::now();
    uint64_t runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    return runningTime;
}

/*---------------------------------------------------------------
TimeDifference
---------------------------------------------------------------*/
float TimeDifference(const uint32_t &t1, const uint32_t &t2)
{
    return static_cast<float>(t2 - t1);
}

const std::string GetCurrentSystemTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%04d-%02d-%02d_%02d-%02d-%02d",   //%04d can be safer !!!
        (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
        (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}
