#include "globals.h"

void printArray(int* an, int start, int len)
{
    for (int i = 0; i < len; ++i) {
        printf("%d ", an[start + i]);
    }
}