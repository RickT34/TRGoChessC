#ifndef _ChessPotH
#define _ChessPotH
#include "ChessBoard.h"

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

#define ChessPotTie(pot, p1, p2) ((pot)->nxtnode[p1] = (p2)) // 节点链接

#define ChessPotClean(pot) ChessPotTie(pot, ChessPotHead, ChessPotTail) // 清空棋子集

#endif