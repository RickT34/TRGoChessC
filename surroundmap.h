#ifndef _surroundmaph
#define _surroundmaph
#include "chessboard.h"
extern Point SurroundList[BLEN];
extern int SurroundListCount;
#define ADDRANGE 2

void SurroundMapInit();
void SurroundMapAddChess();
void SurroundMapRemoveChess();
void PrintSurroundMap();

#endif