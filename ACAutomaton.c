#include "ACAutomaton.h"
#include "Queue.h"
#include <stdlib.h>
struct trienode {
    int id; //这个单词的序号，只有单词的最后的结点才有
    Trie next[TRIEMAXFORK]; //如果为ASCII码可见字符，则为128
    Trie fail; //失败指针
    int matchids[TRIEMAXID];
    int matchidsLen;
};

Trie NewTrie()
{
    Trie re = (Trie)malloc(sizeof(struct trienode));
    re->fail = NULL;
    re->id = -1;
    re->matchidsLen=0;
    for (int i = 0; i < TRIEMAXFORK; ++i)
        re->next[i] = NULL;
    return re;
}

void FreeTrie(Trie root){
    for(int i=0;i<TRIEMAXFORK;++i){
        if(root->next[i]!=NULL)FreeTrie(root->next[i]);
    }
    free(root);
}

void TrieInsert(Trie root,const char* key,const int len,const int idx) //将字符串key放入字段树,idx>=0
{
    #ifdef DEBUG
    printHead("Inserted : ");
    for(int i=0;i<len;++i){
        printf("%d",(int)(key[i]));
    }
    printf(" id=%d\n",idx);
    #endif
    for (int i = 0; i < len; i++) {
        int k = key[i];
        if (root->next[k] == NULL) {
            root->next[k] = NewTrie();
        }
        root = root->next[k];
    }
    root->id = idx;
}
void TrieCompile2(Trie tr, Trie root)
{
    Trie temp = tr;
    while (temp != root) {
        if (temp->id != -1)
            tr->matchids[tr->matchidsLen++] = temp->id;
        temp = temp->fail;
    }
    for (int i = 0; i < TRIEMAXFORK; ++i) {
        if (tr->next[i] != NULL)
            TrieCompile2(tr->next[i], root);
    }
}
void TrieCompile(Trie root) //用bfs建立失配指针
{
    Queue q = NewQueue(100);
    QueuePushback(q, root);
    while (!QueueEmpty(q)) {
        Trie temp = QueueFront(q);
        QueuePopfront(q);
        for (int i = 0; i < TRIEMAXFORK; i++)
            if (temp->next[i]) {
                if (temp == root)
                    temp->next[i]->fail = root;
                else { //兄弟结点之间不能建立失败指针，所以不断回溯，如果该结点的
                    //父节点的失败指针对应的结点next[i]存在,则将这个结点的失败指针连到这里
                    //回溯到root都没有，则结点的失败指针直接指向root
                    Trie p = temp->fail;
                    while (p) {
                        if (p->next[i]) {
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
    TrieCompile2(root, root);
}


void TrieQuery(const char* strin,const int step,const int len,const Trie root, int* ret)
{
    Trie p = root;
    for (int i = 0; i < len; i++, strin+=step) {
        int k = *strin;
        while (p->next[k]==NULL && p != root)
            p = p->fail;
        p = p->next[k];
        if (p == NULL)
            p = root;
        // Trie temp = p;
        // while (temp != root) {
        //     if (temp->id != -1)
        //         ret[temp->id]++;
        //     temp = temp->fail;
        // }
        for (int j = 0; j < p->matchidsLen; ++j) {
            ret[p->matchids[j]] += 1;
        }
    }
}

void TrieQuery2(const char* strin,const int step,const int len,const Trie root, int* ret[])
{
    Trie p = root;
    for (int i = 0; i < len; i++, strin += step) {
        int k = *strin;
        while (p->next[k]==NULL && p != root)
            p = p->fail;
        p = p->next[k];
        if (p == NULL)
            p = root;
        Trie temp = p;
        while (temp != root) {
            if (temp->id != -1)
                ret[i][temp->id]=1;
            temp = temp->fail;
        }
        for (int j = 0; j < p->matchidsLen; ++j) {
            ret[i][p->matchids[j]] += 1;
        }
    }
}
// int main()
// {
//     while (scanf("%d", &n)) {
//         Trie root = NewNode();
//         int idx = 0;
//         for (int i = 1; i <= n; i++) {
//             scanf("%s", strx[i]);
//             idx++;
//             inserttrie(root, strx[i], idx);
//         }
//         buildfail(root);
//         scanf("%s", strin);
//         TrieQuery(root);
//     }
//     return 0;
// }