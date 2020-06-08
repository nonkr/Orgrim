#ifndef __SYSUTIL_H__
#define __SYSUTIL_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdlib>
#include <openssl/md5.h>

class SysUtil
{
public:
    static void _mkdir(const char *dir)
    {
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
    }

    static int MkdirIfNotExists(const char *path)
    {
        struct stat sb;
        if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
        {
        }
        else
        {
            _mkdir(path);
        }

        return 0;
    }

    static int RecursiveDeleteDir(const char *path)
    {
        DIR    *d       = opendir(path);
        size_t path_len = strlen(path);
        int    r        = -1;

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

        return r;
    }

    /*
     * char md5string[33];
     * SysUtil::GetMD5String((unsigned char *) stAlgDataIn.amplitued_normal, sizeof(short) * MPTOF_PIXELS, md5string);
     * printf("LineDetected amplitued_normal md5:[%s]\n", md5string);
     */
    static void GetMD5String(const unsigned char *pBin, size_t sLen, char *pMD5String)
    {
        unsigned char digest[16];

        MD5(pBin, sLen, digest);

        for (int i = 0; i < 16; ++i)
        {
            sprintf(&pMD5String[i * 2], "%02X", (unsigned int) digest[i]);
        }
    }

    /*
     * char digest[16];
     * SysUtil::GetMD5Digest((unsigned char *) stAlgDataIn.amplitued_normal, sizeof(short) * MPTOF_PIXELS, digest);
     */
    static unsigned char *GetMD5Digest(const unsigned char *pBin, size_t sLen, unsigned char *digest)
    {
        return MD5(pBin, sLen, digest);
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
};

#endif //__SYSUTIL_H__
