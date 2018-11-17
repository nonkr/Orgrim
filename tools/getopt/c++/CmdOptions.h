#ifndef _CMDOPTIONS_H_
#define _CMDOPTIONS_H_

struct AppOptions_t
{
    int  nBackground;
    int  nVerbose;
    int  nArgOnlyLongOption1;
    int  nArgOnlyLongOption2;
    int  nArgOnlyShortOption;
    int  nArgLongAndShortOption;
    int  nArgWithParameter;
    char *pStringArg;
    int  bBoolean1;
    int  bBoolean2;
    int  bBoolean3;
};

extern AppOptions_t G_AppOptions;

int ParseCmdOptions(int argc, char *argv[]);

#endif //_CMDOPTIONS_H_
