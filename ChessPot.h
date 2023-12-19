#ifndef _ChessPotH
#define _ChessPotH
#include "ChessBoard.h"

#define ChessPotNULL -1
#define ChessPotHead BLEN
#define ChessPotTail (BLEN + 1)

typedef struct {
    Point nxtnode[BLEN + 2];
} * ChessPot;

ChessPot NewChessPot();
void FreeChessPot(ChessPot pot);

#define ChessPotTie(pot, p1, p2) ((pot)->nxtnode[p1] = (p2))
void ChessPotAdd(ChessPot pot, const Point p);
// void ChessPotRemove(ChessPot pot, const Point p);
#define ChessPotClean(pot) ChessPotTie(pot, ChessPotHead, ChessPotTail)

#endif