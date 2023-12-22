#ifndef _QueueH
#define _QueueH
/********************双端队列数据类型************************/
typedef void *queueitem;      // 元素类型
typedef struct _queue *Queue; // 双端队列
struct _queue
{
    queueitem *items; // 队列元素
    int head;         // 头指针
    int tile;         // 尾指针
    int capbility;    // 容量
};
#define QueueFront(queue) (queue->items[queue->head])  // 队头元素
#define QueueEmpty(queue) (queue->head == queue->tile) // 判断队列是否为空

/// @brief 队头入队
/// @param q 双端队列
/// @param item 元素
void QueuePushfront(Queue q, queueitem item);
/// @brief 队尾入队
/// @param q 双端队列
/// @param item 元素
void QueuePushback(Queue q, queueitem item);
/// @brief 获取队尾元素
/// @param q 双端队列
/// @return 队尾元素
queueitem QueueBack(Queue q);
/// @brief 队头出队
/// @param q 队头元素
void QueuePopfront(Queue q);
/// @brief 队尾出队
/// @param q 队尾元素
void QueuePopback(Queue q);
/// @brief 实例化空双端队列
/// @param capability 容量
/// @return 空双端队列
Queue NewQueue(int capability);
/// @brief 释放双端队列
/// @param q 双端队列
void FreeQueue(Queue q);

#endif