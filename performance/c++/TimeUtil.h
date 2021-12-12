#ifndef _TIMEUTIL_H_
#define _TIMEUTIL_H_

#include <iostream>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;
using namespace chrono;

#define TICK(X) auto X = system_clock::now();
#define GTICK(X) do { \
                    X = system_clock::now(); \
                 } while(false);
#define TOCK(X, LABEL) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    printf("TDO %s: %f s\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define GTOCK(X, LABEL) TOCK(X, LABEL)
#define TOCK_R(X, LABEL) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    X = system_clock::now(); \
                    printf("TDO %s: %f s\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                } while(false);
#define GTOCK_R(X, LABEL) TOCK_R(X, LABEL)
#define TOCK_IF(X, LABEL, R) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    auto f = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                    if (f >= R) \
                        FR_PRINT_RED("Time", "TDO %s: %f s\n", LABEL, f); \
                } while(false);
#define GTOCK_IF(X, LABEL, R) TOCK_IF(X, LABEL, R)
#define TOCK_IF_ELSE(X, LABEL, R) do { \
                    auto end      = system_clock::now(); \
                    auto duration = duration_cast<microseconds>(end - (X)); \
                    auto f = double(duration.count()) * microseconds::period::num / microseconds::period::den; \
                    if (f >= R) \
                        FR_PRINT_RED("Time", "TDO %s: %f s\n", LABEL, f); \
                    else \
                        printf("TDO %s: %f s\n", LABEL, f); \
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
                            printf("TDB %s: %f s\n", LABEL, double(duration.count()) * microseconds::period::num / microseconds::period::den); \
                         } \
                       } while (false)

#define FR_DURATION_AVG(x, LABEL) do { \
                         static int i##x = 0; \
                         static double total##x = 0; \
                         static int count##x = 0; \
                         static double min##x = 999999999; \
                         static double max##x = 0; \
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
                            if (d < min##x) min##x = d; \
                            if (d > max##x) max##x = d; \
                            printf("[%s] TDB %s: %f s, min: %f s, max: %f s, avg: %f s\n", TimeUtil::TimeNowStringMS().c_str(), LABEL, d, min##x, max##x, total##x / count##x); \
                         } \
                       } while (false)

#define HIK_SYSTIME_PROC_PATH   "/proc/hik_systime"

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
};

#endif //_TIMEUTIL_H_
