#ifndef _AIUtilitiesH
#define _AIUtilitiesH

#include "ACAutomaton.h"
#include "Stack.h"
#include "ChessBoard.h"
#include "Globals.h"

/*******************AI算法相关数据结构*******************************/

/**********************棋力图相关***********************************/
#define POWERSLEN ((LLN * 3 - 1) * 2) // 行、列、斜、反斜的整行棋力值个数
typedef struct
{
    Power *linePower[BLEN][4]; // 棋盘上每一个点所在的4条直线的棋力评分指针
    Power powers[POWERSLEN];   // 棋盘上每一条直线的棋力评分
    Power powerSum;            // 棋力评分总和
} *PowerMap;                   // 棋力图，保存并维护棋盘上每一条直线的棋力评分

/// @brief 实例化空白棋力图
/// @return 空白棋力图
PowerMap NewPowerMap();
/// @brief 释放棋力图
/// @param pm 棋力图
void FreePowerMap(PowerMap pm);
/// @brief 打印棋力图
/// @param pm 棋力图
void PrintPowerMap(PowerMap pm);

PowerMap ClonePowerMap(const PowerMap pm);

/******************棋子集相关***************************/
#define ChessPotNULL -1         // 棋子集空节点
#define ChessPotHead BLEN       // 棋子集头节点
#define ChessPotTail (BLEN + 1) // 棋子集尾节点

typedef struct
{
    Point nxtnode[BLEN + 2];
} *ChessPot; // 棋子集，采用数组化的单链表，实现O(1)随机访问，O(1)单点删除，O(1)端点区间删除

/// @brief 实例化空棋子集
/// @return 空棋子集
ChessPot NewChessPot();
/// @brief 释放棋子集
/// @param pot 棋子集
void FreeChessPot(ChessPot pot);
ChessPot CloneChessPot(const ChessPot cp);

#define ChessPotTie(pot, p1, p2) ((pot)->nxtnode[p1] = (p2)) // 节点链接

#define ChessPotClean(pot) ChessPotTie(pot, ChessPotHead, ChessPotTail) // 清空棋子集

/***********************动态邻居表相关*******************************/
typedef struct
{
    char map[BLEN];      // 落子记录图
    ChessPot pot;        // 邻居点集合
    Point history[BLEN]; // 落子历史
    int historyCount;    // 落子历史数
} *NeighborMap;          // 动态邻居表，在线维护已落点的所有邻居点

/// @brief 实例化空白动态邻居表
/// @return 空白动态邻居表
NeighborMap NewNeighborMap();
/// @brief 释放动态邻居表
/// @param nbm 动态邻居表
void FreeNeighborMap(NeighborMap nbm);
NeighborMap CloneNeighborMap(const NeighborMap nbm);
/// @brief 向动态邻居表中落子
/// @param nbm 动态邻居表
/// @param p 落子位置
void NeighborMapAddChess(NeighborMap nbm, Point p);
/// @brief 撤回落子
/// @param nbm 动态邻居表
/// @param s 落子位置
void NeighborMapUndo(NeighborMap nbm, Point s);
/// @brief 从落子历史重建动态邻居表
/// @param nbm 动态邻居表
/// @param actionHistory 落子历史
void NeighborMapFlush(NeighborMap nbm, const Stack actionHistory);
/// @brief 打印动态邻居表
/// @param nbm 动态邻居表
void PrintNeighborMap(NeighborMap nbm);

/********************Zobrist表相关****************************/
#define HASHLENBIT 20 // 哈希表大小
#define HASHMASK ((1 << HASHLENBIT) - 1)
#define HASHLEN (1 << HASHLENBIT)
#define HASHPOOLLEN 4000000
typedef struct
{
    uint64 key;
    Power power;
} HashVal;
typedef struct _hashNode *HashNode;
struct _hashNode
{
    HashVal val;
    HashNode nxt;
};
typedef struct
{
    uint64* turnTable[2];   // 随机数转化表
    uint64 start;                // 起始随机数
    HashNode hashTable[HASHLEN]; // 哈希表
    int count;
    struct _hashNode nodesPool[HASHPOOLLEN];
} *ZobristTable;                 // Zobrist表

/// @brief 实例化空白Zobrist表
/// @return 空白Zobrist表
ZobristTable NewZobristTable();
ZobristTable CloneZobristTable(const ZobristTable zb);
/// @brief 释放Zobrist表
/// @param zt Zobrist表
void FreeZobristTable(ZobristTable zt);
/// @brief Zobrist表查询并插入
/// @param zt Zobrist表
/// @param key 键值
/// @return 如果没有查询到，则将键值插入并返回0；否则不进行任何操作，返回1
HashNode ZobristTableFind(ZobristTable zt, const uint64 key);
void ZobristTableInsert(ZobristTable zt, const uint64 key, const Power power);
#define ZobristNextKey(zt, key, point, player) ((key) ^ (zt->turnTable[player][point])) // 获取下一个键值

#define ZobristClean(zt) memset(zt->hashTable, 0, sizeof(zt->hashTable)),zt->count=0 // 清空哈希表

/**************************AI数据**********************************/
typedef struct
{
    PowerMap powerMap;
    NeighborMap neighborMap;
    Trie patterns;
    int playerid;
    char needflush;
    ZobristTable zobristTable;
} *AIData;

/// @brief 从棋盘重建棋力图
/// @param aidata AI数据
/// @param cb 棋盘
/// @param PatternLen 匹配模式个数
void PowerMapFlush(AIData aidata, const ChessBoard cb, const int PatternLen);

AIData CloneAIData(const AIData aidata);
void PrintAIData(const AIData aidata);
void FreeAIData(AIData aidata);
#endif