#ifndef _QueueH
#define _QueueH
typedef void* queueitem;
typedef struct _queue* Queue;
struct _queue {
    queueitem* items;
    int head;
    int tile;
    int capbility;
};
#define QueueFront(queue) (queue->items[queue->head])
#define QueueEmpty(queue) (queue->head == queue->tile)
void QueuePushfront(Queue q, queueitem item);
void QueuePushback(Queue q, queueitem item);
queueitem QueueBack(Queue q);
void QueuePopfront(Queue q);
void QueuePopBack(Queue q);
Queue NewQueue(int capability);

#endif