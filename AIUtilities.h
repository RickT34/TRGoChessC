#ifndef _AIUtilitiesH
#define _AIUtilitiesH

#include "ACAutomaton.h"
#include "ChessPot.h"
#include "Stack.h"
#include "chessboard.h"

#define POWERSLEN ((LLN * 3 - 1) * 2)
#define SETPower(pm, point, d, power) pm->powerSum += power - *(pm->linePower)[point][d], *(pm->linePower)[point][d] = power
typedef int Power;
typedef struct {
    Power* linePower[BLEN][4];
    Power powers[POWERSLEN];
    Power powerSum;
} * PowerMap;

typedef struct {
    char map[BLEN];
    ChessPot pot;
    Stack history;
    char needflush;
} * NeighborMap;

typedef struct {
    PowerMap powerMap;
    NeighborMap neighborMap;
    Trie patterns;
    Power* patternPowers;
} * AIData;
void NeighborMaptest();
void PowerMaptest();
PowerMap NewPowerMap();
void PrintPowerMap(PowerMap pm);
NeighborMap NewNeighborMap();
void NeighborMapAddChess(NeighborMap nbm, Point p);
void NeighborMapUndo(NeighborMap nbm, Point s);
void NeighborMapFlush(NeighborMap nbm, const Stack actionHistory);
void PrintNeighborMap(NeighborMap nbm);

#endif