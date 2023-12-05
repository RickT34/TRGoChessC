#ifndef _PQueueH
#define _PQueueH
typedef int pqueueitem;
struct _pqueue {
    pqueueitem* items;
    int count;
    int capbility;
};
typedef struct _pqueue *PQueue;
#define PQueueTop(pq) (pq->items[1])
#define PQEmpty(pq) (pq->count==0)
#define PQSize(pq) (pq->count)

void PQueuePop(PQueue pq);
void PQueuePush(PQueue pq, pqueueitem item);
PQueue NewPQueue(int capbility);

#endif