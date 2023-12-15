#ifndef _ChessPotH
#define _ChessPotH
#include "chessboard.h"

#define ChessPotNULL -1
#define ChessPotHead BLEN
#define ChessPotTail (BLEN+1)

typedef struct{
    Point pre;
    Point nxt;
} PotNode;

typedef struct{
    PotNode nodes[BLEN+2];
} *ChessPot;

ChessPot NewChessPot();
void FreeChessPot(ChessPot pot);


#define ChessPotTie(pot, p1, p2) pot->nodes[p2].pre = p1, pot->nodes[p1].nxt = p2
void ChessPotAdd(ChessPot pot, const Point p);
void ChessPotRemove(ChessPot pot, const Point p);
#define ChessPotClean(pot) ChessPotTie(pot, ChessPotHead,ChessPotTail)

#endif