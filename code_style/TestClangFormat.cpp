
#include "TestClangFormat.h"

void AlignTrailingComments() {
    int a;     // My comment a
    int b = 2; // comment  b
}

void AllowAllArgumentsOnNextLine(int a, int b, int c, int d) {}

void AllowShortBlocksOnASingleLine() {
    while (true) {}
    while (true) {
        continue;
    }
}

void AllowShortCaseLabelsOnASingleLine()
{

}

int main() {
    int aaa = bbbbbbbbbbbbbbb + ccccccccccccccc;

    int aaaaaaaaaaaaaaa;
    int bbbbbbbbbbbbbbb;
    int ccccccccccccccc;
    int ddddddddddddddd;
    AllowAllArgumentsOnNextLine(aaaaaaaaaaaaaaa, bbbbbbbbbbbbbbb,
                                ccccccccccccccc, ddddddddddddddd);
    return 0;
}
