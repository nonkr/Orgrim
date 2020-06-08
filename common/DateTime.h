#ifndef  __DATE_TIME_H__
#define  __DATE_TIME_H__

/***********************************************************************************************************************
*
* ��Ȩ��Ϣ����Ȩ���� (c) , ���ݺ����������ּ������޹�˾, ��������Ȩ��
*
* �ļ����ƣ�DateTime.h
* �ļ���ʶ��
* ժ    Ҫ��ʱ����غ�����װ

* ��ǰ�汾��1.0.1
* ��    �ߣ�������
* ��    �ڣ�2020��1��
* ��    ע��
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

    //��ʼ��ʱ
    uint64_t Tic(); //ʵ����������mLastTimeΪ��ǰʱ��

                   //�����ϴ�Start������ʱ��ʵ���ϵ��õ���Update();
    uint64_t Tac();

    //��ȡ���ʱ����
    uint64_t GetMaxInterval();
    //��ȡ��Сʱ����
    uint64_t GetMinInterval();
    //��ȡ���µ�ʱ����
    uint64_t GetLastInterval();
    //��ȡƽ��ʱ����
    uint64_t GetAverageInterval();
private:
    uint64_t mMaxInterval, mMinInterval, mLastInterval;
    uint64_t mLastIntervals[MAX_AVERAGE_COUNT];
    uint64_t mLastTime;
    int32_t mIndex;

    //������ ��ʱ������
    //���µ�ǰʱ�䣬���ϴθ��µ�ʱ����������ʱ�䡣
    //�������ϴε���Update֮���ʱ����
    uint64_t Update();

    //������ ��ʱ�ڲ�ʹ��
    //����last_time�͵�ǰʱ��֮��ļ�����������ڲ���mLastTime
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

//��õ�ǰ��ʱ���

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
