#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "csv.h"

struct EvInfo
{
    char arrDateTime[20];
    int  evState;
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
        memcpy(info.arrDateTime, oneline.at(0).c_str(), 20);
        info.voltage = atoi(oneline.at(4).c_str());
        a.emplace_back(info);
    }
    fp.close();
    return 0;
}

int Calc(EvInfo &info, time_t &tLastEpoch, FILE *fp)
{
    const char *pFormat        = "%Y-%m-%d %H:%M:%S"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html
    struct tm  tmNow           = {0};
    char       arrDuration[20] = {0};

    strptime(info.arrDateTime, pFormat, &tmNow);
    time_t    duration = mktime(&tmNow);
    time_t    diff     = tLastEpoch - duration;
    struct tm *ptr     = localtime(&diff);
    strftime(arrDuration, 64, "%H:%M:%S", ptr);
    fprintf(fp, "%d,%s,%ld\n", info.voltage, arrDuration, diff);
    return 0;
}

int main(int argc, char *argv[])
{
    vector<EvInfo> info;

    ReadFile(argv[1], info);

    setenv("TZ", "GMT", 1);
    tzset();

    time_t     tLastEpoch = 0;
    const char *pFormat   = "%Y-%m-%d %H:%M:%S"; // reference: http://man7.org/linux/man-pages/man3/strptime.3.html

    printf("last:%s,%d\n", info.at(info.size() - 1).arrDateTime, info.at(info.size() - 1).voltage);

    struct tm tmLast = {0};
    strptime(info.at(info.size() - 1).arrDateTime, pFormat, &tmLast);
    tLastEpoch = mktime(&tmLast);

    FILE *fp = fopen("recharge_time_cost.csv", "w");
    fprintf(fp, "from_voltage,duration,cost\n");
    for (EvInfo n : info)
    {
        Calc(n, tLastEpoch, fp);
    }
    fclose(fp);

    return 0;
}
