#include <stdio.h>
#include <zlog.h>

int main(int argc, char **argv)
{
    int rc;
    rc = dzlog_init("a.conf", "my_cat");
    if (rc)
    {
        printf("init failed\n");
        return -1;
    }
    dzlog_info("hello, zlog aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    dzlog_info("hello, zlog bbbbbbbbbbb\n");
    zlog_fini();
    return 0;
}
