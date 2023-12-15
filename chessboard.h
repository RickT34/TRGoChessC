#ifndef _chessboardH
#define _chessboardH
#include "globals.h"

typedef char ChessType;
const extern ChessType PlayerChessTypes[2];
#define LLN 15
#define FULL (ChessType)3
#define PlayerW (ChessType)1 // 01 O
#define PlayerB (ChessType)2 // 10 #
#define BLANK (ChessType)0
#define BIT 2

#define POINTBITS 4
#define BWIDTH (1 << (POINTBITS))
#define BLEN (BWIDTH * (BWIDTH - 1))
#define KEYXMASK ((1 << POINTBITS) - 1) // b00001111
#define KEYYMASK (KEYXMASK << POINTBITS) // b11110000

typedef char* ChessBoard;
typedef int Point;
#define PointNULL -1

/*   Chess Board View
      0 x--         15
  y 0 00000000000000X --TB  D
  | 1 00000000000000X      /
    2 00000000000000X
    . ...         ...
   14 00000000000000X --BB
 C    |            |
  \   LB           RB
*/
#define GetPoint(x, y) (((y) << POINTBITS) | (x))
#define GetPointX(point) ((point)&KEYXMASK)
#define GetPointY(point) ((point) >> POINTBITS)
#define PointToXY(point, x, y) \
    x = GetPointX(point);      \
    y = GetPointY(point);
// s=x+y
#define GetPointS(point) (((point)&KEYXMASK) + ((point) >> POINTBITS))
// d=x-y
#define GetPointD(point) (((point)&KEYXMASK) - ((point) >> POINTBITS))

#define GetChessXY(chessboard, x, y) ((chessboard)[GetPoint(x, y)])
#define SetChessXY(chessboard, x, y, v) GetChessXY(chessboard, x, y) = v
#define GetChess(chessboard, point) chessboard[point]
#define SetChess(chessboard, point, v) GetChess(chessboard, point) = v

#define XSTEP 1
#define YSTEP (1 << POINTBITS)
#define CSTEP (YSTEP + XSTEP)
#define DSTEP (YSTEP - XSTEP)

/*d from [X, Y, C, D]*/
#define Next(point, d) point += d##STEP

#define IsInLB(point) (((point)&KEYXMASK) == 0)
#define IsInRB(point) (((point)&KEYXMASK) == (LLN - 1))
#define IsInTB(point) (((point)&KEYYMASK) == 0)
#define IsInBB(point) (((point)&KEYYMASK) == ((LLN - 1) << POINTBITS))

#define IsLegal(point) ((point) >= 0 && (point) < BLEN && ((point)&KEYXMASK != LLN))
#define IsLegalXY(x, y) ((x) >= 0 && (y) >= 0 && (x) < LLN && (y) < LLN)

ChessBoard NewChessBoard();
ChessBoard CloneChessBoard(ChessBoard ct);
int ChessBoardSave(ChessBoard ct, char* file);
int ChessBoardLoad(ChessBoard *ct, char* file);
void FreeChessBoard(ChessBoard ct);

typedef char* ChessBoardStyle[12];
const extern ChessBoardStyle ChessBoardStyle_Classic;
void PrintChessBoard(const ChessBoard ct,const ChessBoardStyle style);
const char* GetChessSkin(ChessType type,const ChessBoardStyle style);

const extern char DireNames[4];
const extern int DireSteps[4];
#define DireLen 4
typedef struct {
    Point start[4]; //[X, Y, C, D]
    int lens[4];
} * ChessBoardInf;
ChessBoardInf NewChessBoardInf();
extern ChessBoardInf CBINF;
void FreeChessBoardInf(ChessBoardInf ctn);
void PrintPointInf(const ChessBoardInf ctinf,const Point p);

typedef struct{
    Point neighbors[17];
    int len;
} * ChessBoardNeighbor;
ChessBoardNeighbor NewChessBoardNeighbor();
extern ChessBoardNeighbor CBNEI;

void ChessBoardInit();

#ifdef DEBUG
#define PointTo2C(point) ((point) >> POINTBITS) + 1, ((point)&KEYXMASK) + 'a' // exp: 5d
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