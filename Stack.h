#ifndef _StackH
#define _StackH
typedef int stackitem;
typedef struct _stack {
    int Count;
    int Capability;
    stackitem* Items;
} *Stack;

#define StackPush(stack, v) stack->Items[stack->Count++]=v
#define StackPop(stack) stack->Items[--stack->Count]
#define StackTop(stack) stack->Items[stack->Count-1]

Stack NewStack(int capability);
void FreeStack(Stack stack);
void PrintStack(Stack stack);

#endif