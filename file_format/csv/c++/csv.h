#ifndef __CSV_H__
#define __CSV_H__

#include <vector>
#include <string>

using namespace std;

class CSV
{
public:
    static int ParseLine(const char *pStr, vector<string> &oneline);
};

#endif //__CSV_H__
