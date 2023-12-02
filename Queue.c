#include "Queue.h"
//[head,tile)
void QueuePushfront(Queue q, queueitem item)
{
    if (q->head == 0)
        q->head = q->capbility - 1;
    else
        q->head -= 1;
    q->items[q->head] = item;
}

void QueuePushback(Queue q, queueitem item)
{
    q->items[q->tile] = item;
    if (q->tile == q->capbility - 1)
        q->tile = 0;
    else
        q->tile += 1;
}

void QueuePopback(Queue q)
{
    if (q->tile == 0)
        q->tile = q->capbility - 1;
    else
        q->tile -= 1;
}

void QueuePopfront(Queue q)
{
    if (q->head == q->capbility - 1)
        q->head = 0;
    else
        q->head += 1;
}

queueitem QueueBack(Queue q)
{
    if (q->tile == 0)
        return q->items[q->capbility - 1];
    else
        return q->items[q->tile - 1];
}

Queue NewQueue(const int capbility)
{
    Queue re = (Queue)malloc(sizeof(struct _queue));
    re->capbility = capbility;
    re->items = (queueitem*)malloc(sizeof(queueitem) * capbility);
    re->head = 0;
    re->tile = 0;
    return re;
}