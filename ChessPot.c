#include "ChessPot.h"

ChessPot NewChessPot()
{
    ChessPot re = malloc(sizeof(*re));
    re->nodes[ChessPotHead].nxt = ChessPotTail;
    re->nodes[ChessPotTail].pre = ChessPotHead;
    return re;
}

void FreeChessPot(ChessPot pot)
{
    free(pot);
}

// void ChessPotAdd(ChessPot pot, const Point p)
// { // assert(!pot.count(p))
//     pot->nxtnode[p] = pot->nxtnode[ChessPotHead];
//     pot->nxtnode[ChessPotHead] = p;
//     pot->nodes[p].nxt
// }

// void ChessPotRemove(ChessPot pot, const Point p)
// { // assert(pot.count(p))
//     PotNode n = pot->nodes[p];
//     pot->nodes[n.pre].nxt = n.nxt;
//     pot->nodes[n.nxt].pre = n.pre;
//     ChessPotTie(pot,pot->nodes[p].pre,pot->nodes[p].nxt);
// }
