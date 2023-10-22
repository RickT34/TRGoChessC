#ifndef _LinkedListH
#define _LinkedListH
#include <stdlib.h>

typedef int LinkedListItem;
typedef struct _listnode* ListNode;
struct _listnode {
    LinkedListItem val;
    ListNode nxt;
    char deleted;
};
typedef struct _linkedlist* LinkedList;

#define EndNode NULL
LinkedList NewLinkedList(const int capability);
ListNode ListNodeInsertAfter(const LinkedList list, const ListNode node, const LinkedListItem val);
ListNode ListNodeNext(const ListNode node);
void ListFix(const LinkedList list);
void ListNodeRemove(const LinkedList list, const ListNode node);

#endif