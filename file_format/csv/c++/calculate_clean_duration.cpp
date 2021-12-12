#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "csv.h"

struct EvInfo
{
    char arrDateTime[20];
    int  voltage;
};

int ReadFile(char *pFile, vector<EvInfo> &a)
{
    ifstream fp(pFile);

    if (!fp.is_open())
    {
        fprintf(stderr, "open %s failed\n", pFile);
        return 2;
    }

    bool           bSkipFirstFile = false;
    vector<string> oneline;

    for (string line; getline(fp, line);)
    {
        if (!bSkipFirstFile)
        {
            bSkipFirstFile = true;
            continue;
        }
        CSV::ParseLine(line.c_str(), oneline);

        EvInfo info;
        memcpy(info.arrDateTime, oneline.at(1).c_str(), 20);
        info.voltage = atoi(oneline.at(10).c_str());
        a.emplace_back(info);
    }
    fp.close();
    return 0;
}

int Calc(EvInfo &info, time_t &tFirstEpoch, FILE *fp)
{
    const char *pFormat        = "%Y-%m-%d %H:%M:%S"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    struct tm  tmNow           = {0};
    char       arrDuration[20] = {0};
    static int voltage143Count = 0;
    static bool printed143 = false;
    static int voltage140Count = 0;
    static bool printed140 = false;

    strptime(info.arrDateTime, pFormat, &tmNow);
    time_t    duration = mktime(&tmNow);
    time_t    diff     = duration - tFirstEpoch;
    struct tm *ptr     = localtime(&diff);
    strftime(arrDuration, 64, "%H:%M:%S", ptr);
//    printf("%s,%d,%s,%ld\n", info.arrDateTime, info.voltage, arrDuration, diff);
    if (info.voltage < 14300)
    {
        voltage143Count++;
        if (voltage143Count == 30)
        {
            printf("To bat 14.3v >>> %s,%d,%s,%ld\n", info.arrDateTime, info.voltage, arrDuration, diff);
            printed143 = true;
        }
    }
    else if (!printed143)
    {
        voltage143Count = 0;
    }
    if (info.voltage < 14000)
    {
        voltage140Count++;
        if (voltage140Count == 10)
        {
            printf("To bat 14.0v >>> %s,%d,%s,%ld\n", info.arrDateTime, info.voltage, arrDuration, diff);
            printed140 = true;
        }
    }
    else if (!printed140)
    {
        voltage140Count = 0;
    }

    fprintf(fp, "%s,%d,%s,%ld\n", info.arrDateTime, info.voltage, arrDuration, diff);
    return 0;
}

int main(int argc, char *argv[])
{
    vector<EvInfo> info;

    ReadFile(argv[1], info);

    setenv("TZ", "GMT", 1);
    tzset();

    time_t     tFirstEpoch = 0;
    const char *pFormat    = "%Y-%m-%d %H:%M:%S"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html

    printf("Clean from:%s,%d\n", info.at(0).arrDateTime, info.at(0).voltage);

    struct tm tmFirst = {0};
    strptime(info.at(0).arrDateTime, pFormat, &tmFirst);
    tFirstEpoch = mktime(&tmFirst);

    FILE *fp = fopen("clean_duration.csv", "w");
    fprintf(fp, "from_datetime,from_voltage,duration,cost\n");
    for (EvInfo n : info)
    {
        Calc(n, tFirstEpoch, fp);
    }
    fclose(fp);

    return 0;
}
