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

#define CTICK(X) struct timeval X; gettimeofday(&X, NULL);
#define CTOCK_AVG(X, LABEL) do { \
                static double total##x = 0; \
                static int count##x = 0; \
                struct timeval now; \
                gettimeofday(&now, NULL); \
                double d = (now.tv_sec - X.tv_sec) + double(now.tv_usec - X.tv_usec) / 1000000; \
                count##x++; \
                total##x += d; \
                FR_ALOG_YELLOW("Time", "TDO %s: %f s, avg: %f s\n", LABEL, d, total##x / count##x); \
                } while(false);

#define TICK(X) auto X = system_clock::now();
#define GTICK(X) do { \
                    X = system_clock::now(); \
                 } while(false);
#define TOCK(X, LABEL) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    FR_ALOG_YELLOW("Time", "TDO %s: %f s\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define TOCK_AVG(X, LABEL) do { \
                    static double total##x = 0; \
                    static int count##x = 0; \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    count##x++; \
                    double d = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                    total##x += d; \
                    FR_ALOG_YELLOW("Time", "TDO %s: %f s, avg: %f s\n", LABEL, d, total##x / count##x); \
                } while(false);
#define GTOCK(X, LABEL) TOCK(X, LABEL)
#define GTOCK_AVG(X, LABEL) TOCK_AVG(X, LABEL)
#define TOCK_R(X, LABEL) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    X = system_clock::now(); \
                    FR_ALOG_YELLOW("Time", "TDO %s: %f s\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define GTOCK_R(X, LABEL) TOCK_R(X, LABEL)
#define TOCK_IF(X, LABEL, R) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    auto f = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                    if (f >= R) \
                        {FR_ALOG_RED("Time", "TDO %s: %f s\n", LABEL, f);} \
                } while(false);
#define GTOCK_IF(X, LABEL, R) TOCK_IF(X, LABEL, R)
#define TOCK_IF_ELSE(X, LABEL, R) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    auto f = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                    if (f >= R) \
                        {FR_ALOG_RED("Time", "TDO %s: %f s\n", LABEL, f);} \
                    else \
                        {FR_ALOG_YELLOW("Time", "TDO %s: %f s\n", LABEL, f);} \
                } while(false);
#define GTOCK_IF_ELSE(X, LABEL, R) TOCK_IF_ELSE(X, LABEL, R)
#define GCLOCK system_clock::time_point

#define FR_DURATION(x, LABEL) do { \
                         static int i##x = 0; \
                         static system_clock::time_point old; \
                         if (i##x == 0) \
                         { \
                            i##x = 1; \
                            old = system_clock::now(); \
                         } \
                         else \
                         { \
                            system_clock::time_point now = system_clock::now(); \
                            auto duration = duration_cast<microseconds>(now - old); \
                            old = now; \
                            FR_PRINT_MAGENTA("Time", "TDB %s: %f s\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                         } \
                       } while (false)

#define FR_DURATION_AVG(x, LABEL) do { \
                         static int i##x = 0; \
                         static double total##x = 0; \
                         static int count##x = 0; \
                         static system_clock::time_point old; \
                         if (i##x == 0) \
                         { \
                            i##x = 1; \
                            old = system_clock::now(); \
                         } \
                         else \
                         { \
                            system_clock::time_point now = system_clock::now(); \
                            auto duration = duration_cast<microseconds>(now - old); \
                            old = now; \
                            count##x++; \
                            double d = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                            total##x += d; \
                            FR_PRINT_MAGENTA("Time", "TDB %s: %f s, avg: %f s\n", LABEL, d, total##x / count##x); \
                         } \
                       } while (false)

#define FR_DURATION_AVG_IF(x, LABEL, R) do { \
                         static int i##x = 0; \
                         static double total##x = 0; \
                         static int count##x = 0; \
                         static system_clock::time_point old; \
                         if (i##x == 0) \
                         { \
                            i##x = 1; \
                            old = system_clock::now(); \
                         } \
                         else \
                         { \
                            system_clock::time_point now = system_clock::now(); \
                            auto duration = duration_cast<microseconds>(now - old); \
                            old = now; \
                            count##x++; \
                            double d = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                            total##x += d; \
                            if (d >= R) \
                                {FR_PRINT_RED("Time", "TDB %s: %f s, avg: %f s\n", LABEL, d, total##x / count##x);} \
                         } \
                       } while (false)

#define HIK_SYSTIME_PROC_PATH   "/proc/hik_systime"

class TimeUtil
{
public:
    static std::string TimeNowString()
    {
        auto tp = system_clock::now();

        auto ttime_t = system_clock::to_time_t(tp);

        std::tm *ttm = localtime(&ttime_t);

        char time_str[] = "yyyy-mm-dd HH:MM:SS";

        strftime(time_str, sizeof(time_str), "%F %T", ttm);

        string result(time_str);

        return result;
    }

    static int TimeNowSeconds()
    {
        auto tp = system_clock::now();

        auto ttime_t = system_clock::to_time_t(tp);

        std::tm *ttm = localtime(&ttime_t);

        char time_str[] = "HH:MM:SS";

        strftime(time_str, sizeof(time_str), "%T", ttm);

        int hour, minute, second;
        sscanf(time_str, "%d:%d:%d", &hour, &minute, &second);

        return hour * 60 * 60 + minute * 60 + second;
    }

    static std::string TimeNowStringMS()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm *pTime;
        pTime = localtime(&tv.tv_sec);

        char sTemp[32] = {0};
        snprintf(sTemp, sizeof(sTemp), "%04d-%02d-%02d %02d:%02d:%02d.%06ld", pTime->tm_year + 1900,
                 pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec,
                 tv.tv_usec);
        return (string) sTemp;
    }

    static std::string TimePointToString(std::chrono::system_clock::time_point &tp)
    {
        auto ttime_t = system_clock::to_time_t(tp);

        std::tm *ttm = localtime(&ttime_t);

        char time_str[] = "yyyy-mm-dd HH:MM:SS";

        strftime(time_str, sizeof(time_str), "%F %T", ttm);

        string result(time_str);

        return result;
    }

    static unsigned long long HikSysTime()
    {
        char buf[80] = {0};
        int  count;

        int fd = open(HIK_SYSTIME_PROC_PATH, O_RDONLY);
        if (fd < 0)
        {
            FR_ALOG_RED("Time", "Failed to open %s\n", HIK_SYSTIME_PROC_PATH);
            return 0;
        }
        count = read(fd, buf, 80);
        if (count < 0)
        {
            FR_ALOG_RED("Time", "Error reading from %s\n", HIK_SYSTIME_PROC_PATH);
            return 0;
        }
        close(fd);

        return atoll(buf);
    }

    static bool TimeDurationGT(system_clock::time_point &checkPoint, int duration)
    {
        system_clock::time_point timeNow = system_clock::now();
        auto                     d       = duration_cast<microseconds>(timeNow - checkPoint);
        double                   d2      = (double(d.count()) * microseconds::period::num / microseconds::period::den);
        return d2 > duration;
    }
};

#endif //_TIMEUTIL_H_
