#include "Stack.h"
#include <stdlib.h>
#include <stdio.h>

Stack NewStack(int capability){
    Stack re=(Stack)malloc(sizeof(struct _stack));
    re->Capability=capability;
    re->Count=0;
    re->Items=(stackitem*)malloc(sizeof(stackitem)*capability);
    return re;
}

void FreeStack(Stack stack){
    free(stack->Items);
    free(stack);
}

void PrintStack(Stack stack){
    for(int i=0;i<stack->Count;++i){
        printf("%d ",stack->Items[i]);
    }
    printf("\n");
}