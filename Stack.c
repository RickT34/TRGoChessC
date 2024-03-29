#include "Stack.h"
#include <stdio.h>
#include <stdlib.h>

Stack NewStack(int capability)
{
    Stack re = (Stack)malloc(sizeof(struct _stack));
    re->Capability = capability;
    re->Count = 0;
    re->Items = (stackitem *)malloc(sizeof(stackitem) * capability);
    return re;
}

void FreeStack(Stack stack)
{
    free(stack->Items);
    free(stack);
}