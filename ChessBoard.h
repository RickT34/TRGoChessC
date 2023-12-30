#ifndef _ChessBoardH
#define _ChessBoardH
#include "Globals.h"

/*********************棋子类别定义***************************/
/// @brief 棋盘上的棋子类别
typedef char ChessType;
/// @brief 玩家棋子类别
const extern ChessType PlayerChessTypes[2];
#define LLN 15                 // 棋盘宽度
#define PlayerW ((ChessType)1) // 白棋
#define PlayerB ((ChessType)2) // 黑棋
#define BLANK ((ChessType)0)   // 空白

/************************点类型定义**************************/
typedef unsigned char Point;                      // 将XY坐标压缩成一个int来代表位置点
#define POINTBITS 4                               // 一个坐标所占比特数，必须满足 (1<<POINTBITS) >= LLN
#define KEYXMASK ((1 << POINTBITS) - 1)           // X坐标掩码
#define KEYYMASK (KEYXMASK << POINTBITS)          // Y坐标掩码
#define PointNULL 0xff                            // NULL点标记
#define GetPoint(x, y) (((y) << POINTBITS) | (x)) // 获取位置点

/************************棋盘定义***************************/
/*       棋盘示意图
      0 X--         15
  Y 0 00000000000000X --TB  D
  | 1 00000000000000X      /
    2 00000000000000X
    . ...         ...
   14 00000000000000X --BB
 C    |            |
  \   LB           RB
*/
typedef char *ChessBoard;               // 棋盘，使用一维char数组存储
#define BLEN ((1 << (POINTBITS)) * LLN) // 棋盘大小

#define GetChessXY(chessboard, x, y) ((chessboard)[GetPoint(x, y)])      // 获取xy位置上的棋子类别
#define SetChessXY(chessboard, x, y, v) GetChessXY(chessboard, x, y) = v // 设置xy位置上的棋子类别
#define GetChess(chessboard, point) chessboard[point]                    // 获取point位置上的棋子类别
#define SetChess(chessboard, point, v) GetChess(chessboard, point) = v   // 设置point位置上的棋子类别

#define XSTEP 1                // 向X方向走一步的下标增量
#define YSTEP (1 << POINTBITS) // 向Y方向走一步的下标增量
#define CSTEP (YSTEP + XSTEP)  // 向C方向走一步的下标增量
#define DSTEP (YSTEP - XSTEP)  // 向D方向走一步的下标增量

#define IsLegal(point) ((point) >= 0 && (point) < BLEN && ((point) & KEYXMASK != LLN)) // 判断point位置是否在棋盘内
#define IsLegalXY(x, y) ((x) >= 0 && (y) >= 0 && (x) < LLN && (y) < LLN)               // 判断xy位置是否在棋盘内

/// @brief 实例化一个空白棋盘
/// @return 空白棋盘
ChessBoard NewChessBoard();
/// @brief 克隆一个棋盘
/// @param ct 要克隆的棋盘
/// @return 棋盘的克隆
ChessBoard CloneChessBoard(const ChessBoard ct);
/// @brief 释放棋盘
/// @param ct 要释放的棋盘
void FreeChessBoard(ChessBoard ct);
/// @brief 保存棋盘到字符串
/// @param ct 要保存的棋盘
/// @param file 要写入的字符串，需要保证容量>=BLEN
/// @return 写入的字符数
int ChessBoardSave(ChessBoard ct, char *file);
/// @brief 从字符串读取棋盘
/// @param ct 存放棋盘的位置，不用预先实例化
/// @param file 要读取的字符串
/// @return 读取的字符数
int ChessBoardLoad(ChessBoard *ct, char *file);

/*************************棋盘打印相关***********************/
typedef char *ChessBoardStyle[12];                    // 棋盘样式类型
const extern ChessBoardStyle ChessBoardStyle_Classic; // 默认的棋盘样式

/// @brief 打印棋盘
/// @param ct 要打印的棋盘
/// @param style 使用的棋盘样式
void PrintChessBoard(const ChessBoard ct, const ChessBoardStyle style);
/// @brief 获取棋盘样式中的棋子样式
/// @param type 棋子类别
/// @param style 棋盘样式
/// @return 棋子样式
const char *GetChessSkin(ChessType type, const ChessBoardStyle style);

/************************遍历表相关**************************/
#define DireLen 4               // 方向数
const extern char DireNames[4]; // 方向名
const extern int DireSteps[4];  // 方向增量

typedef struct
{
    Point start[4]; // 遍历起始位置，顺序 [X, Y, C, D]
    int lens[4];    // 遍历长度
} *ChessBoardInf;   // 遍历表，存储棋盘上每个点在4个方向上的遍历信息

/// @brief 实例化新的遍历表
/// @return 遍历表
ChessBoardInf NewChessBoardInf();
extern ChessBoardInf CBINF; // 已实例化的遍历表

/// @brief 释放遍历表
/// @param ctn 要释放的遍历表
void FreeChessBoardInf(ChessBoardInf ctn);
/// @brief 打印一个位置上的遍历信息
/// @param ctinf 遍历表
/// @param p 位置
void PrintPointInf(const ChessBoardInf ctinf, const Point p);

/***********************邻居表相关*****************************/
#define NEIGHBORRANGE 1 // 延伸宽度
typedef struct
{
    Point neighbors[8 * NEIGHBORRANGE]; // 邻居数量
    int len;                            // 邻居个数
} *ChessBoardNeighbor;                  // 邻居表，储存棋盘上点的相邻几个需要进行搜索的点

/// @brief 实例化新的邻居表
/// @return 新的邻居表
ChessBoardNeighbor NewChessBoardNeighbor();
extern ChessBoardNeighbor CBNEI; // 已实例化的邻居表

/**************************杂项*******************************/

/// @brief 棋盘类相关初始化
void ChessBoardInit();

#define PointTo2C(point) ((point) >> POINTBITS) + 1, ((point) & KEYXMASK) + 'a' // 将点表示成数字+字母形式

#ifdef DEBUG
#define printRow(chessboard, startpoint, step, len)                                                                                                                                 \
    printfD("Start From %d%c, step %d, length %d.\n    ", PointTo2C(startpoint), step, len) for (int _i = 0; _i < len; _i++) printf("%1d ", chessboard[startpoint + (_i) * (len)]); \
    printf("\n")
#define printPoint(point, header) printfD(header "point: %d%c", PointTo2C(point))
#define printPointList(list, count)                               \
    printfD("List= %s, Count= %d", #list, count);                 \
    printf("  [");                                                \
    for (int _i = 0; _i < (count); _i++)                          \
        printf(_i == 0 ? "%d%c" : ", %d%c", PointTo2C(list[_i])); \
    printf("]\n");
#endif

#endif