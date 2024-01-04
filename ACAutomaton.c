#include "ACAutomaton.h"
#include "Queue.h"
#include <stdlib.h>
struct trienode
{
    int id;
    Trie next[TRIEMAXFORK];
    Trie ed[TRIEMAXFORK];
    Trie fail;
    Power powerAdd;
};

Trie NewTrie()
{
    Trie re = (Trie)malloc(sizeof(struct trienode));
    re->fail = NULL;
    re->id = -1;
    re->powerAdd = 0;
    for (int i = 0; i < TRIEMAXFORK; ++i)
        re->next[i] = re->ed[i] = NULL;
    return re;
}

void FreeTrie(Trie root)
{
    for (int i = 0; i < TRIEMAXFORK; ++i)
    {
        if (root->ed[i] != NULL)
        {
            FreeTrie(root->ed[i]);
        }
    }
    free(root);
}

void TrieInsert(Trie root, const char *key, const int len, const int idx) // 将字符串key放入字段树,idx>=0
{
#ifdef DEBUG
    printHead("Inserted : ");
    for (int i = 0; i < len; ++i)
    {
        printf("%d", (int)(key[i]));
    }
    printf(" id=%d\n", idx);
#endif
    for (int i = 0; i < len; i++)
    {
        int k = key[i];
        if (root->next[k] == NULL)
        {
            root->ed[k] = root->next[k] = NewTrie();
        }
        root = root->next[k];
    }
    root->id = idx;
}
void TrieCompile2(Trie tr, Trie root, const Power *powers) // 用dfs计算节点权值
{
    Trie temp = tr;
    while (temp != root)
    {
        if (temp->id != -1)
            tr->powerAdd += powers[temp->id];
        temp = temp->fail;
    }
    for (int i = 0; i < TRIEMAXFORK; ++i)
    {
        if (tr->next[i] != NULL)
            TrieCompile2(tr->next[i], root, powers);
        else
        {
            if (tr == root)
            {
                tr->next[i] = root;
            }
            else
            {
                temp = tr;
                while (temp->next[i] == NULL && temp != root)
                    temp = temp->fail;
                tr->next[i] = temp->next[i];
            }
        }
    }
}
void TrieCompile(Trie root, const Power *powers) // 用bfs建立失配指针，节点的失配指针指向前缀节点的nxt指针
{
    Queue q = NewQueue(100);
    QueuePushback(q, root);
    while (!QueueEmpty(q))
    {
        Trie temp = QueueFront(q);
        QueuePopfront(q);
        for (int i = 0; i < TRIEMAXFORK; i++)
            if (temp->next[i])
            {
                if (temp == root)
                    temp->next[i]->fail = root;
                else
                {
                    Trie p = temp->fail;
                    while (p)
                    {
                        if (p->next[i])
                        {
                            temp->next[i]->fail = p->next[i];
                            break;
                        }
                        p = p->fail;
                    }
                    if (p == NULL)
                        temp->next[i]->fail = root;
                }
                QueuePushback(q, temp->next[i]);
            }
    }
    FreeQueue(q);
    TrieCompile2(root, root, powers);
}

Power TrieQuery(const char *strin, const int step, const int len, const Trie root) // 自动机
{
    Trie p = root;
    Power ret = 0;
    for (int i = 0; i < len; i++, strin += step)
    {
        p = p->next[*strin];
        ret += p->powerAdd;
    }
    return ret;
}