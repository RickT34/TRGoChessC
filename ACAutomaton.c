#include "ACAutomaton.h"
#include "Queue.h"
#include <stdlib.h>
struct trienode {
    int id; //这个单词的序号，只有单词的最后的结点才有
    Trie next[TRIEMAXFORK]; //如果为ASCII码可见字符，则为128
    Trie fail; //失败指针
};

Trie NewTrie()
{
    Trie re = (Trie)malloc(sizeof(struct trienode));
    re->fail = NULL;
    re->id = -1;
    for (int i = 0; i < TRIEMAXFORK; ++i)
        re->next[i] = NULL;
    return re;
}

void TrieInsert(Trie root, char* key, int len, int idx) //将字符串key放入字段树,idx>=0
{
    // #ifdef DEBUG
    // printHead("key");
    // for(int i=0;i<len;++i){
    //     printf("%d",(int)(key[i]));
    // }
    // printf("\n");
    // #endif
    for (int i = 0; i < len; i++) {
        int k = key[i];
        if (root->next[k] == NULL) {
            root->next[k] = NewTrie();
        }
        root = root->next[k];
    }
    root->id = idx;
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
}
void TrieQuery(char* strin, int step, int len, Trie root, int* ret)
{
    Trie p = root;
    for (int i = 0; i < len; i++) {
        int k = strin[i];
        while (!p->next[k] && p != root)
            p = p->fail;
        p = p->next[k];
        if (p == NULL)
            p = root;
        Trie temp = p;
        while (temp != root) {
            if (temp->id != -1)
                ret[temp->id]++;
            temp = temp->fail;
        }
    }
}

void TrieQuery2(char* strin, int step, int len, Trie root, int* ret[])
{
    Trie p = root;
    for (int i = 0; i < len; i++) {
        int k = strin[i];
        while (!p->next[k] && p != root)
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