#ifndef __SYSUTIL_H__
#define __SYSUTIL_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <cstdlib>
#include <cstdint>

#include <sys/prctl.h>
#include <dirent.h>
#include <unistd.h>

class SysUtil
{
public:
    static void _mkdir(const char *dir)
    {
#ifdef RK
        char   tmp[256];
        char   *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp), "%s", dir);
        len = strlen(tmp);
        if (tmp[len - 1] == '/')
            tmp[len - 1] = 0;
        for (p = tmp + 1; *p; p++)
            if (*p == '/')
            {
                *p = 0;
                mkdir(tmp, S_IRWXU);
                *p = '/';
            }
        mkdir(tmp, S_IRWXU);
#endif
    }

    static int MkdirIfNotExists(const char *path)
    {
#ifdef RK
        struct stat sb;
        if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
        {
        }
        else
        {
            _mkdir(path);
        }
#endif
        return 0;
    }

    static int RecursiveDeleteDir(const char *path)
    {
        int r = -1;

#ifdef RK
        DIR    *d       = opendir(path);
        size_t path_len = strlen(path);

        if (d)
        {
            struct dirent *p;
            r = 0;

            while (!r && (p = readdir(d)))
            {
                int    r2 = -1;
                char   *buf;
                size_t len;

                /* Skip the names "." and ".." as we don't want to recurse on them. */
                if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                {
                    continue;
                }

                len = path_len + strlen(p->d_name) + 2;
                buf = (char *) malloc(len);

                if (buf)
                {
                    struct stat statbuf;

                    snprintf(buf, len, "%s/%s", path, p->d_name);

                    if (!stat(buf, &statbuf))
                    {
                        if (S_ISDIR(statbuf.st_mode))
                        {
                            r2 = RecursiveDeleteDir(buf);
                        }
                        else
                        {
                            r2 = unlink(buf);
                        }
                    }

                    free(buf);
                }

                r = r2;
            }

            closedir(d);
        }

        if (!r)
        {
            r = rmdir(path);
        }
#endif
        return r;
    }


    static inline bool FileExists(char *pFileName)
    {
        struct stat buffer;
        return (stat(pFileName, &buffer) == 0);
    }

    static inline int FileSize(char *pFileName)
    {
        struct stat sb;
        if (stat(pFileName, &sb) == -1)
        {
            return -1;
        }
        return sb.st_size;
    }

    static uint16_t CalcChecksum(void *dataptr, uint16_t len)
    {
        uint32_t acc;
        uint16_t src;
        uint8_t  *octetptr;

        acc      = 0;
        octetptr = (uint8_t *) dataptr;
        while (len > 1)
        {
            src = (*octetptr) << 8;
            octetptr++;
            src |= (*octetptr);
            octetptr++;
            acc += src;
            len -= 2;
        }
        if (len > 0)
        {
            src = (*octetptr) << 8;
            acc += src;
        }

        acc = (acc >> 16) + (acc & 0x0000FFFFUL);
        if ((acc & 0xFFFF0000UL) != 0)
        {
            acc = (acc >> 16) + (acc & 0x0000FFFFUL);
        }

        src = (uint16_t) acc;
        return ~src;
    }

    static void SetThreadName(const char *pName)
    {
#ifdef RK
        prctl(PR_SET_NAME, pName);
#endif
    }
};

#endif //__SYSUTIL_H__
