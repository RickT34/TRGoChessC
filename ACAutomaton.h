#ifndef _ACAutomatonH
#define _ACAutomatonH
#include "Globals.h"
#define TRIEMAXFORK 3
#define TRIEMAXID 20
typedef struct trienode* Trie;
Trie NewTrie();
void FreeTrie(Trie root);
void TrieInsert(Trie root,const char* key,const int len,const int idx);
void TrieCompile(Trie root);
void TrieQuery(const char* strin,const int step,const int len,const Trie root, int* ret);
void TrieQuery2(const char* strin,const int step,const int len,const Trie root, int* ret[]);
#endif