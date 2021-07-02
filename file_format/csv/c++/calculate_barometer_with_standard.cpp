#include <iostream>
#include <fstream>
#include <cstring>
#include <string.h>
#include "csv.h"

struct BarometerInfo
{
    int  index;
    char arrDateTime[20];
    int  seconds;
    char fan[8];
    int  barometer;
    int  diff;
    int  battery;
};

struct StandardBarometerElement
{
    char arrDateTime[20];
    int  seconds;
    int  barometer;
};

struct StandardBarometerInfo
{
    int                      count = 0;
    StandardBarometerElement arrElement[10];
};

int ReadFile(char *pFile, vector<BarometerInfo> &a)
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

        int i = atoi(oneline.at(2).c_str());
        a.at(i).index = atoi(oneline.at(0).c_str());
        memcpy(a.at(i).arrDateTime, oneline.at(1).c_str(), 20);
        a.at(i).seconds = i;
        memcpy(a.at(i).fan, oneline.at(3).c_str(), strlen(oneline.at(3).c_str()));
        a.at(i).barometer = atoi(oneline.at(4).c_str());
        a.at(i).diff      = atoi(oneline.at(5).c_str());
        a.at(i).battery   = atoi(oneline.at(6).c_str());
    }
    fp.close();
    return 0;
}

/**
 * 读取静止状态下的标准大气压值，多台设备采集时，可多次读取，拟合曲线
 * @param pFile
 * @param a
 * @return
 */
int ReadStandardFile(char *pFile, vector<StandardBarometerInfo> &a)
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

        int i = atoi(oneline.at(2).c_str());                                                // 第三列(seconds)
        memcpy(a.at(i).arrElement[a.at(i).count].arrDateTime, oneline.at(1).c_str(), 20);   // 第二列(datetime)
        a.at(i).arrElement[a.at(i).count].seconds   = i;
        a.at(i).arrElement[a.at(i).count].barometer = atoi(oneline.at(4).c_str());          // 第五列(当前大气压)
        a.at(i).count++;
    }
    fp.close();
    return 0;
}

int Merge(BarometerInfo &curr, vector<StandardBarometerInfo> &standard, FILE *fp)
{
    for (StandardBarometerInfo n : standard)
    {
        for (int i = 0; i < n.count; i++)
        {
            if (n.arrElement[i].seconds == curr.seconds &&
                strncmp(n.arrElement[i].arrDateTime, curr.arrDateTime, 19) == 0)
            {
                fprintf(fp, "%s,%d,%s,%d,%d,%d,%d,%d\n",
                        curr.arrDateTime, curr.seconds, curr.fan, n.arrElement[i].barometer, curr.barometer, curr.diff,
                        n.arrElement[i].barometer - curr.barometer, curr.battery);
//            printf("datetime:%s seconds:%d fan:%s barometer_standard:%d barometer:%d diff_orig:%d diff_true:%d battery:%d\n",
//                   curr.arrDateTime, curr.seconds, curr.fan,
//                   n.barometer, curr.barometer, curr.diff,
//                   n.barometer - curr.barometer, curr.battery);
                return 0;
            }
        }
    }

    printf("not found, datetime:%s seconds:%d barometer:%d battery:%d\n", curr.arrDateTime, curr.seconds,
           curr.barometer, curr.battery);
    return 1;
}

int main(int argc, char *argv[])
{
    vector<BarometerInfo>         orig(86400);
    vector<StandardBarometerInfo> standard(86400);

    ReadFile(argv[1], orig);                // 风机工作时采集到的样本
    ReadStandardFile(argv[2], standard);    // 风机停止时采集到的样本1
    ReadStandardFile(argv[3], standard);    // 风机停止时采集到的样本2

//    char arrFilename[128] = {0};
//    sprintf(arrFilename, "%s_merged.csv", basename(argv[1]));

    FILE *fp = fopen("merged.csv", "w");
    fprintf(fp, "datetime,seconds,fan,barometer_standard,barometer,diff_orig,diff_true,battery\n");
    for (BarometerInfo n : orig)
    {
        if (n.battery != 0)
        {
            Merge(n, standard, fp);
        }
    }
    fclose(fp);

    return 0;
}
