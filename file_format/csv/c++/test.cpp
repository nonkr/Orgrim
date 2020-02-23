#include <iostream>
#include <fstream>
#include "csv.h"

int main(int argc, char *argv[])
{
    ifstream fp(argv[1]);

    if (!fp.is_open())
    {
        fprintf(stderr, "open %s failed\n", argv[1]);
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
        printf("%s\n", oneline.at(0).c_str());
    }
    fp.close();
    return 0;
}
