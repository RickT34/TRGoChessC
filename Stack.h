#ifndef _StackH
#define _StackH
/******************栈数据类型**********************/
typedef void *stackitem; // 元素类型
typedef struct _stack
{
    int Count;        // 元素数量
    int Capability;   // 容量
    stackitem *Items; // 元素
} *Stack;             // 栈

#define StackPush(stack, v) stack->Items[stack->Count++] = v // 元素入栈
#define StackPop(stack) stack->Items[--stack->Count]         // 元素出栈
#define StackTop(stack) stack->Items[stack->Count - 1]       // 栈顶元素
#define StackClean(stack) stack->Count = 0;

/// @brief 实例化空栈
/// @param capability 容量
/// @return 空栈
Stack NewStack(int capability);
/// @brief 释放栈
/// @param stack 栈
void FreeStack(Stack stack);

#endif