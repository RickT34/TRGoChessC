#include "LinkedList.h"
#include "Stack.h"
#include <stdlib.h>

struct _linkedlist {
    ListNode head;
    int Count;
    ListNode* pool;
    int capability;
    ListNode* deletedpool;
    int deletedpoolcount;
};

LinkedList NewLinkedList(const int capability)
{
    LinkedList re = (LinkedList)malloc(sizeof(struct _linkedlist));
    re->head = EndNode;
    re->Count = 0;
    re->capability = capability;
    re->pool = (ListNode*)malloc(sizeof(struct _listnode) * capability);
    re->deletedpool = (ListNode*)malloc(sizeof(struct _listnode) * capability);
    re->deletedpoolcount=0;
    return re;
}

ListNode ListNodeInsertAfter(const LinkedList list, const ListNode node, const LinkedListItem val)
{
    ListNode re;
    if(list->deletedpoolcount>0)
    {
        re = list->deletedpool[--list->deletedpoolcount];
    }
    else{
        re=list->pool[list->Count];
    }
    ++list->Count;
    re->nxt=node->nxt;
    node->nxt=re;
    re->deleted=0;
    re->val=val;
    return re;
}

ListNode ListNodeNext(const ListNode node)
{
    ListNode re=node->nxt;
    while(re!=EndNode&&re->deleted)re=re->nxt;
    node->nxt=re;
    return re;
}

void ListFix(const LinkedList list){
    ListNode n=list->head,nxt;
    while(n!=EndNode){
        nxt=n->nxt;
        while(nxt!=EndNode&&nxt->deleted==1)nxt=nxt->nxt;
        n->nxt=nxt;
        n=nxt;
    }
}

void ListNodeRemove(const LinkedList list, const ListNode node)
{
    node->deleted=1;
    list->deletedpool[list->deletedpoolcount++]=node;
}