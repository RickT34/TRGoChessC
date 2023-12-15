#include "ChessPot.h"

ChessPot NewChessPot()
{
    ChessPot re = malloc(sizeof(*re));
    re->nodes[ChessPotHead].nxt=ChessPotTail;
    re->nodes[ChessPotHead].pre=ChessPotNULL;
    re->nodes[ChessPotTail].pre=ChessPotHead;
    re->nodes[ChessPotTail].nxt=ChessPotNULL;
    return re;
}

void FreeChessPot(ChessPot pot)
{
    free(pot);
}

void ChessPotAdd(ChessPot pot,const Point p)
{ // assert(!pot.count(p))
    Point n = pot->nodes[ChessPotHead].nxt;
    pot->nodes[n].pre = p;
    pot->nodes[p].nxt = n;
    pot->nodes[p].pre=ChessPotHead;
    pot->nodes[ChessPotHead].nxt=p;
}

void ChessPotRemove(ChessPot pot,const Point p)
{ // assert(pot.count(p))
    PotNode n = pot->nodes[p];
    pot->nodes[n.pre].nxt=n.nxt;
    pot->nodes[n.nxt].pre=n.pre;
}

