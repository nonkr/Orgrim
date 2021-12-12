#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <climits>

struct SwpCoor
{
    short sCoorX;   //XPos
    short sCoorY;   //YPos
    short usDegree; //Degree
};

int main()
{
    SwpCoor m_stLastSwpPath;
    m_stLastSwpPath.sCoorX = 20;
    m_stLastSwpPath.sCoorY = 100;

    SwpCoor stSwpCoor;
    stSwpCoor.sCoorX = 21;
    stSwpCoor.sCoorY = 130;

    if (m_stLastSwpPath.sCoorX != SHRT_MAX && m_stLastSwpPath.sCoorY != SHRT_MAX)
    {
        int absX = abs(m_stLastSwpPath.sCoorX - stSwpCoor.sCoorX);
        int absY = abs(m_stLastSwpPath.sCoorY - stSwpCoor.sCoorY);
        if (absX > 1 || absY > 1)
        {
            printf("Abnormal path coor, x:(%d,%d,%d), y:(%d,%d,%d)\n", m_stLastSwpPath.sCoorX,
                   stSwpCoor.sCoorX, absX, m_stLastSwpPath.sCoorY, stSwpCoor.sCoorY, absY);
            if (absX >= absY)
            {
                if (m_stLastSwpPath.sCoorX < stSwpCoor.sCoorX)
                {
                    for (short x = m_stLastSwpPath.sCoorX + 1; x < stSwpCoor.sCoorX; x++)
                    {
                        short y = abs(x - m_stLastSwpPath.sCoorX) * (absY - 1) / (absX - 1);
                        if (m_stLastSwpPath.sCoorY == stSwpCoor.sCoorY)
                        {
                            y = m_stLastSwpPath.sCoorY;
                        }
                        else if (m_stLastSwpPath.sCoorY < stSwpCoor.sCoorY)
                        {
                            y = m_stLastSwpPath.sCoorY + 1 + y;
                        }
                        else
                        {
                            y = m_stLastSwpPath.sCoorY - 1 - y;
                        }

                        printf("x:%d y:%d\n", x, y);
                    }
                }
                else
                {
                    for (short x = m_stLastSwpPath.sCoorX - 1; x > stSwpCoor.sCoorX; x--)
                    {
                        short y = abs(x - m_stLastSwpPath.sCoorX) * (absY - 1) / (absX - 1);
                        if (m_stLastSwpPath.sCoorY == stSwpCoor.sCoorY)
                        {
                            y = m_stLastSwpPath.sCoorY;
                        }
                        else if (m_stLastSwpPath.sCoorY < stSwpCoor.sCoorY)
                        {
                            y = m_stLastSwpPath.sCoorY + 1 + y;
                        }
                        else
                        {
                            y = m_stLastSwpPath.sCoorY - 1 - y;
                        }

                        printf("x:%d y:%d\n", x, y);
                    }
                }
            }
            else
            {
                if (m_stLastSwpPath.sCoorY < stSwpCoor.sCoorY)
                {
                    for (short y = m_stLastSwpPath.sCoorY + 1; y < stSwpCoor.sCoorY; y++)
                    {
                        short x = abs(y - m_stLastSwpPath.sCoorY) * (absX - 1) / (absY - 1);
                        if (m_stLastSwpPath.sCoorX == stSwpCoor.sCoorX)
                        {
                            x = m_stLastSwpPath.sCoorX;
                        }
                        else if (m_stLastSwpPath.sCoorX < stSwpCoor.sCoorX)
                        {
                            x = m_stLastSwpPath.sCoorX + 1 + x;
                        }
                        else
                        {
                            x = m_stLastSwpPath.sCoorX - 1 - x;
                        }

                        printf("x:%d y:%d\n", x, y);
                    }
                }
                else
                {
                    for (short y = m_stLastSwpPath.sCoorY - 1; y > stSwpCoor.sCoorY; y--)
                    {
                        short x = abs(y - m_stLastSwpPath.sCoorY) * (absX - 1) / (absY - 1);
                        if (m_stLastSwpPath.sCoorX == stSwpCoor.sCoorX)
                        {
                            x = m_stLastSwpPath.sCoorX;
                        }
                        else if (m_stLastSwpPath.sCoorX < stSwpCoor.sCoorX)
                        {
                            x = m_stLastSwpPath.sCoorX + 1 + x;
                        }
                        else
                        {
                            x = m_stLastSwpPath.sCoorX - 1 - x;
                        }

                        printf("x:%d y:%d\n", x, y);
                    }
                }
            }
        }
    }

    return 0;
}
