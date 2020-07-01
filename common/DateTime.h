#ifndef  __DATE_TIME_H__
#define  __DATE_TIME_H__

/***********************************************************************************************************************
*
* 版权信息：版权所有 (c) , 杭州海康威视数字技术有限公司, 保留所有权利
*
* 文件名称：DateTime.h
* 文件标识：
* 摘    要：时间相关函数封装

* 当前版本：1.0.1
* 作    者：王晓佳
* 日    期：2020年1月
* 备    注：
*
***********************************************************************************************************************/

#include <inttypes.h>
#include <vector>
#include <thread>
#include <string>
#include <chrono>

#ifndef _WINDOWS
#define Sleep(X) std::this_thread::sleep_for(std::chrono::milliseconds((int)(X)))
#endif

#ifdef _WINDOWS
#include <Windows.h>
#endif // _WINDOWS

#define INVALID_TIMESTAMP 0
#define MAX_AVERAGE_COUNT 16

#define ENABLE_TIME_TRACE 1
#define MAX_TIME_TRACE_INSTANCE 4
#define MAX_TIME_TRACE_ITEMS 64

#if ENABLE_TIME_TRACE
#define START_TIME_TRACE(x) TimeTracer::Instance(x)->Start()
#define STOP_TIME_TRACE(x) TimeTracer::Instance(x)->Stop()
#define ADD_TRACE_POINT(x) TimeTracer::Instance(x)->AddTracePoint(__FILE__, __FUNCTION__, __LINE__)
#else
#define START_TIME_TRACE(x)
#define STOP_TIME_TRACE(x)
#define ADD_TRACE_POINT(x)
#endif
class TimeInterval {
public:
    TimeInterval();
    ~TimeInterval();

    //开始计时
    uint64_t Tic(); //实际上设置了mLastTime为当前时间

                   //返回上次Start以来的时间差，实际上调用的是Update();
    uint64_t Tac();

    //获取最大时间间隔
    uint64_t GetMaxInterval();
    //获取最小时间间隔
    uint64_t GetMinInterval();
    //获取最新的时间间隔
    uint64_t GetLastInterval();
    //获取平均时间间隔
    uint64_t GetAverageInterval();
private:
    uint64_t mMaxInterval, mMinInterval, mLastInterval;
    uint64_t mLastIntervals[MAX_AVERAGE_COUNT];
    uint64_t mLastTime;
    int32_t mIndex;

    //有问题 暂时不开放
    //更新当前时间，用上次更新的时间来计算间隔时间。
    //返回与上次调用Update之间的时间间隔
    uint64_t Update();

    //有问题 暂时内部使用
    //计算last_time和当前时间之间的间隔，并更新内部的mLastTime
    uint64_t Update(uint64_t last_time);
};

class TimeTracer 
{
public:
    ~TimeTracer();

    static TimeTracer *Instance(int32_t id);

    void Start();
    void Stop();
    void AddTracePoint(const char *file, const char *function, int32_t line);

private:
    TimeTracer(int32_t instance_id);
    static TimeTracer *mInstance[MAX_TIME_TRACE_INSTANCE];

    struct TimeTraceItem {
        char file_name[128];
        char function_name[64];
        int32_t line;
        uint64_t time_stamp;
    };

    TimeTraceItem *mTimeTraceItems;
    int32_t mTimeTraceItemIndex;
    uint64_t mStartTime;
    int32_t mInstanceId;
};

//获得当前的时间戳

typedef uint64_t TimeStamp;
typedef uint64_t TimeStampNew;
uint64_t GetSystemTickSec();
uint64_t GetSystemTickMs();
uint64_t GetSystemTick();
uint64_t GetCurrentTick();
uint32_t GetCurrentTickSec();
uint64_t GetCurrentTickMs();
inline TimeStampNew Now()
{
    return GetCurrentTick();
}

float TimeDifference(const uint32_t &t1, const uint32_t &t2);
const std::string GetCurrentSystemTime();

#endif
