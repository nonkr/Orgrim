#ifndef _CMDOPTIONS_H_
#define _CMDOPTIONS_H_

struct AppOptions_t
{
    int  nBackground            = 0;
    int  nVerbose               = 0;
    int  nArgOnlyLongOption1    = 0;
    int  nArgOnlyLongOption2    = 0;
    int  nArgOnlyShortOption    = 0;
    int  nArgLongAndShortOption = 0;
    int  nArgWithParameter      = 0;
    char *pStringArg            = nullptr;
    int  bBoolean1              = 0;
    int  bBoolean2              = 0;
    int  bBoolean3              = 0;
};

extern AppOptions_t G_AppOptions;

int ParseCmdOptions(int argc, char *argv[]);

#endif //_CMDOPTIONS_H_
