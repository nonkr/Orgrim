/*
 * Copyright (c) 2017, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2017/12/28 20:20
 *
 */

#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

int hexstring_to_bytearray(char *p_hexstring, char **pp_out, int *p_i_out_len)
{
    size_t i;
    size_t i_str_len;
    char   *p_out    = NULL;
    int    i_out_len = 0;
    int    i_offset  = 0;

    if (p_hexstring == NULL)
        return -1;

    i_str_len = strlen(p_hexstring);

    // 先初步对传入的字符串做一遍检查
    if (i_str_len == 2)
        i_out_len = 1;
    else if ((i_str_len - 2) % 3 == 0)
        i_out_len = (i_str_len - 2) / 3 + 1;
    else
    {
        fprintf(stderr, "malformed input hex string\n");
        goto error;
    }

    p_out = (char *) malloc((size_t) i_out_len);
    if (p_out == NULL)
        return -1;

    for (i = 0; i < i_str_len;)
    {
        if (isxdigit(p_hexstring[i]) == 0 || isxdigit(p_hexstring[i + 1]) == 0)
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        sscanf(p_hexstring + i, "%2x", (unsigned int *) &(p_out[i_offset++]));

        if (i + 2 == i_str_len)
            break;

        if (p_hexstring[i + 2] != ' ')
        {
            fprintf(stderr, "malformed input hex string\n");
            goto error;
        }

        i += 3;
    }

    *pp_out      = p_out;
    *p_i_out_len = i_out_len;

    return 0;
error:
    if (p_out)
        free(p_out);
    return -1;
}

int main(int argc, char **argv)
{
    int  nUsartfd;
    char *pDevice = "/dev/ttyUSB0";
//    char *pSendStr = "hello\n";

    nUsartfd = open(pDevice, O_RDWR | O_NOCTTY | O_NDELAY);
    if (nUsartfd == -1)
    {
        perror("open serial failed");
        return -1;
    }

    char *pSendData   = NULL;
    int  iSendDataLen = 0;
    char id           = 0;
    char buff[256]    = "AA 00 20 81 73 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 8E";

    if (hexstring_to_bytearray(buff, &pSendData, &iSendDataLen))
    {
        perror("HexStringToBytearray failed\n");
        return -1;
    }

    while (1)
    {
        pSendData[4]  = id++;
        pSendData[35] = pSendData[4] + pSendData[3];

        int iWriteLen = write(nUsartfd, pSendData, iSendDataLen);
        printf("Send Len:[%d]\n", iWriteLen);
        usleep(20000);
    }

    if (pSendData)
    {
        free(pSendData);
        pSendData = NULL;
    }


    close(nUsartfd);
    return 0;
}
