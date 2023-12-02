#ifndef _ACAutomatonH
#define _ACAutomatonH
#include "globals.h"
#define TRIEMAXFORK 3
typedef struct trienode* Trie;
Trie NewTrie();
void TrieInsert(Trie root, char* key, int len, int idx);
void TrieCompile(Trie root);
void TrieQuery(char* strin, int step, int len, Trie root, int* ret);

#endif