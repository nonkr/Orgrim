//
// Created by songbinbin on 2019/8/22.
//

#include <cstring>
#include "CircleBuffer.h"

CircleBuffer *pCircleBuffer = nullptr;

void *ReadA(void *arg)
{
    int    a       = pCircleBuffer->Sub();
    char   *pDataA = new char[32];
    memset(pDataA, 0x00, 32);
    size_t nSizeA  = 0;

    while (pCircleBuffer)
    {
        if (pCircleBuffer->Read(a, pDataA, &nSizeA) == 0)
        {
            if (nSizeA > 0)
            {
                printf("ReadA [%s]\n", pDataA);
            }
        }
        sleep(2);
    }

    pthread_exit(nullptr);
}


int main()
{
    pCircleBuffer = new CircleBuffer;
    char *pData   = new char[32];

    pthread_t tid1 = 0;
    pthread_create(&tid1, nullptr, ReadA, nullptr);
    sleep(1);

    for (int i = 0; i < 100; i++)
    {
        memset(pData, 0x00, 32);
        snprintf(pData, 32, "%d", i);
        pCircleBuffer->Write(pData, strlen(pData));
        sleep(1);
    }

    sleep(1);

    delete[]pData;
    delete pCircleBuffer;
    pCircleBuffer = nullptr;

//    pthread_join(tid1, nullptr);
}
