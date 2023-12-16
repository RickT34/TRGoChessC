#include "ChessPot.h"

ChessPot NewChessPot()
{
    ChessPot re = malloc(sizeof(*re));
    re->nxtnode[ChessPotHead] = ChessPotTail;
    re->nxtnode[ChessPotTail] = ChessPotNULL;
    return re;
}

void FreeChessPot(ChessPot pot)
{
    free(pot);
}

void ChessPotAdd(ChessPot pot, const Point p)
{ // assert(!pot.count(p))
    pot->nxtnode[p] = pot->nxtnode[ChessPotHead];
    pot->nxtnode[ChessPotHead] = p;
}

// void ChessPotRemove(ChessPot pot, const Point p)
// { // assert(pot.count(p))
//     PotNode n = pot->nxtnode[p];
//     pot->nxtnode[n.pre].nxt = n.nxt;
//     pot->nxtnode[n.nxt].pre = n.pre;
// }
