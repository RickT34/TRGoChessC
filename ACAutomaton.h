
#ifndef _ACAutomatonH
#define _ACAutomatonH
#include "Globals.h"
/*****************AC自动机相关**********************/

#define TRIEMAXFORK 3          // 字典树最大分叉
#define TRIEMAXID 20           // AC自动机上字符串id最大值
typedef struct trienode *Trie; // 字典树(AC自动机)

/// @brief 实例化新的字典树节点
/// @return 新的字典树节点
Trie NewTrie();
/// @brief 释放字典树节点
/// @param root 字典树节点
void FreeTrie(Trie root);
/// @brief 将字符串插入字典树
/// @param root 字典树的根
/// @param key 字符串
/// @param len 字符串长度
/// @param idx 字符串id
void TrieInsert(Trie root, const char *key, const int len, const int idx);
/// @brief 构造AC自动机
/// @param root 字典树的根
void TrieCompile(Trie root, const Power *powers);
/// @brief AC自动机总体查询
/// @param strin 要查询的字符串
/// @param step 下标增量
/// @param len 长度
/// @param root 字典树的根
/// @param ret 记录结果的数组，不会清空数组，容量至少为 TRIEMAXID
Power TrieQuery(const char *strin, const int step, const int len, const Trie root);

#endif