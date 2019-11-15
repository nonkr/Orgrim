/*
 * Copyright (c) 2019, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

int GatewayHex2Dec(char *hex, char *dec)
{
    if (hex == NULL || dec == NULL)
    {
        return 1;
    }

    if (strlen(hex) != 8)
    {
        return 1;
    }

    unsigned int uiIP1 = 0;
    unsigned int uiIP2 = 0;
    unsigned int uiIP3 = 0;
    unsigned int uiIP4 = 0;

    sscanf(hex + 6, "%2x", &uiIP1);
    sscanf(hex + 4, "%2x", &uiIP2);
    sscanf(hex + 2, "%2x", &uiIP3);
    sscanf(hex + 0, "%2x", &uiIP4);
    sprintf(dec, "%d.%d.%d.%d", uiIP1, uiIP2, uiIP3, uiIP4);

    return 0;
}

int main(int argc, char **argv)
{
    FILE    *stream           = NULL;
    char    *line             = NULL;
    size_t  len               = 0;
    ssize_t read              = 0;
    char    *pos1             = NULL;
    char    *pos2             = NULL;
    char    arrDest[32]       = {0};
    char    arrGateway[32]    = {0}; // 0101A8C0
    char    arrGatewayDec[16] = {0}; // 192.168.1.1

    stream = fopen("/proc/net/route", "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1)
    {
        if (strncmp(line, "wlan0", 5) == 0)
        {
            pos1 = strchr(line, '\t');
            if (pos1)
            {
                pos2 = strchr(pos1 + 1, '\t');
                if (pos2)
                {
                    memset(arrDest, 0x00, sizeof(arrDest));
                    memcpy(arrDest, pos1 + 1, pos2 - (pos1 + 1));
                    if (strncmp(arrDest, "00000000", 8) == 0) // default
                    {
                        pos1 = strchr(pos2 + 1, '\t');
                        if (pos1)
                        {
                            memset(arrGateway, 0x00, sizeof(arrGateway));
                            memset(arrGatewayDec, 0x00, sizeof(arrGatewayDec));
                            memcpy(arrGateway, pos2 + 1, pos1 - (pos2 + 1));
                            GatewayHex2Dec(arrGateway, arrGatewayDec);

                            printf("default gateway:[%s]\n", arrGateway);
                            printf("default gateway:[%s]\n", arrGatewayDec);
                        }
                    }
                }
            }
        }
    }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}
