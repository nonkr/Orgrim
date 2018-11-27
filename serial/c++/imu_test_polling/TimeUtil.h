#ifndef _TIMEUTIL_H_
#define _TIMEUTIL_H_

#include <iostream>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include "PrintUtil.h"

using namespace std;
using namespace chrono;

#define TICK(X) auto X = system_clock::now();
#define GTICK(X) do { \
                    X = system_clock::now(); \
                 } while(false);
#define TOCK(X, LABEL) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    FR_PRINT_BLUE("Time duration of %s: %f sec.\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define GTOCK(X, LABEL) TOCK(X, LABEL)
#define GCLOCK system_clock::time_point

class TimeUtil
{
public:
    static std::string TimePointToString(std::chrono::system_clock::time_point &tp)
    {
        auto ttime_t = system_clock::to_time_t(tp);

        std::tm *ttm = localtime(&ttime_t);

        char date_time_format[] = "%Y%m%d%H%M%S";

        char time_str[] = "yyyymmddHHMMSS";

        strftime(time_str, strlen(time_str), date_time_format, ttm);

        string result(time_str);

        return result;
    }

    static void TimeFormat(struct timeval *pTimeVal, char *pBuffer, size_t nBufferSize)
    {
        struct tm *pTM = localtime(&(pTimeVal->tv_sec));
        snprintf(pBuffer, 64, "%04d/%02d/%02d %02d:%02d:%02d.%06ld",
                 pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
                 pTM->tm_hour, pTM->tm_min, pTM->tm_sec, pTimeVal->tv_usec);
    }
};

#endif //_TIMEUTIL_H_
