#include "Globals.h"

const char *PlayerNames[] = {"人类玩家", "电脑玩家"};

void printArray(int *an, int start, int len)
{
    for (int i = 0; i < len; ++i)
    {
        printf("%d ", an[start + i]);
    }
}

int IntSave(int data, char *file)
{
    int mask = 0xf;
    for (int i = 0; i < 8; ++i)
    {
        file[i] = (data & mask) + 'a';
        data >>= 4;
    }
    return 8;
}

int IntLoad(int *data, char *file)
{
    *data = 0;
    for (int i = 0; i < 8; ++i)
    {
        *data <<= 4;
        *data |= file[7 - i] - 'a';
    }
    return 8;
}

void Input(char *buff, const int buffsize)
{
    printf(">:");
    fgets(buff, buffsize, stdin);
}
