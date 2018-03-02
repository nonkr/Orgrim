#include <stdio.h>
#include <stdlib.h>
#include <config.h>

#ifdef HAVE_POW
  #include <math.h>
#else
  #include <MathFunctions.h>
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

const char *VERSION = "version: v" STR(Demo_VERSION_MAJOR) "." STR(Demo_VERSION_MINOR) "." STR(Demo_VERSION_PATCH);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        // print version info
        printf("%s Version %d.%d.%d\n",
               argv[0],
               Demo_VERSION_MAJOR,
               Demo_VERSION_MINOR,
               Demo_VERSION_PATCH);
        printf("Usage: %s base exponent \n", argv[0]);
        return 1;
    }
    double base = atof(argv[1]);
    int exponent = atoi(argv[2]);
    
#ifdef HAVE_POW
    printf("Now we use the standard library. \n");
    double result = pow(base, exponent);
#else
    printf("Now we use our own Math library. \n");
    double result = power(base, exponent);
#endif
    
    printf("%g ^ %d is %g\n", base, exponent, result);
    return 0;
}
