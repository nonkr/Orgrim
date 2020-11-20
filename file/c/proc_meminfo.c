#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE          *meminfo        = fopen("/proc/meminfo", "r");
    char          *line           = NULL;
    size_t        len             = 0;
    ssize_t       read;
    char          *pos            = NULL;
    char          *pos_pre        = NULL;
    char          *name_to_search = NULL;
    int           find_first_space;
    char          value[32]       = {0};
    long long int iValue          = 0;

    if (argc > 1)
    {
        name_to_search = argv[1];
    }
    else
    {
        printf("Usage: %s name_to_search\n", argv[0]);
        return 1;
    }

    while ((read = getline(&line, &len, meminfo)) != -1)
    {
        if (name_to_search && strncmp(name_to_search, line, strlen(name_to_search)) == 0)
        {
//            printf("%s", line);
            pos_pre          = line;
            find_first_space = 0;
            while (1)
            {
                pos = strchr(pos_pre, ' ');
                if (pos)
                {
                    if (find_first_space == 1 && pos - pos_pre > 0)
                    {
                        break;
                    }
                    if (find_first_space == 0) find_first_space = 1;
//                    printf("pos    :[%s", pos);
                    pos_pre = pos + 1;
                }
                else
                {
                    break;
                }
            }
//            printf("pos_old:[%s", pos_pre);
            pos              = strchr(pos_pre, ' ');
            memcpy(value, pos_pre, pos - pos_pre);
            break;
        }
    }
    free(line);
    fclose(meminfo);
    if (strlen(value) > 0)
    {
        iValue = strtoll(value, NULL, 10);
        printf("iValue:[%lld]\n", iValue);
    }

    return EXIT_SUCCESS;
}
