#include "ChessPot.h"

ChessPot NewChessPot()
{
    ChessPot re = malloc(sizeof(*re));
    ChessPotTie(re, ChessPotHead, ChessPotTail);
    return re;
}

void FreeChessPot(ChessPot pot)
{
    free(pot);
}
