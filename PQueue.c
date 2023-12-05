#include "PQueue.h"
#include <stdlib.h>

static void sink(int k, PQueue pq)
{
    while (k * 2 <= pq->count) {
        int j = (k << 1);
        if (j < pq->count && pq->items[j] < pq->items[j + 1])
            ++j;
        if (pq->items[k] >= pq->items[j])
            break;
        pqueueitem t = pq->items[k];
        pq->items[k] = pq->items[j];
        pq->items[j] = t;
        k = j;
    }
}

void PQueuePop(PQueue pq)
{
    pq->items[1] = pq->items[pq->count--];
    sink(1, pq);
}

static void swim(int k, PQueue pq)
{
    while (k > 1 && pq->items[k] > pq->items[k >> 1]) {
        pqueueitem t = pq->items[k];
        pq->items[k] = pq->items[k >> 1];
        pq->items[k >> 1] = t;
        k >>= 1;
    }
}

void PQueuePush(PQueue pq, pqueueitem item)
{
    pq->items[++pq->count] = item;
    swim(pq->count, pq);
}

PQueue NewPQueue(int capbility)
{
    PQueue re = malloc(sizeof(struct _pqueue));
    re->items = malloc(sizeof(pqueueitem) * capbility);
    re->capbility = capbility;
    re->count = 0;
    return re;
}

// int main(){
//     PQueue pq=NewPQueue(100);
//     int t;
//     int x;
//     while (scanf("%d",&t)==1) {
//         switch (t) {
//             case 1:
//                 scanf("%d",&x);
//                 PQueuePush(pq, x);
//                 break;
//             case 2:
//                 PQueuePop(pq);
//                 break;
//         }
//         printf("%d\n",PQueueTop(pq));
//     }
// }