#ifndef _AIUtilitiesH
#define _AIUtilitiesH

#include "ACAutomaton.h"
#include "ChessPot.h"
#include "Stack.h"
#include "chessboard.h"
#include "globals.h"

#define POWERSLEN ((LLN * 3 - 1) * 2)
#define SETPower(pm, point, d, power) pm->powerSum += power - *(pm->linePower)[point][d], *(pm->linePower)[point][d] = power
typedef struct {
    Power* linePower[BLEN][4];
    Power powers[POWERSLEN];
    Power powerSum;
    char needflush;
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
    int playerid;
} * AIData;
void NeighborMaptest();
void PowerMaptest();
PowerMap NewPowerMap();
void FreePowerMap(PowerMap pm);
void PrintPowerMap(PowerMap pm);
void PowerMapFlush(AIData aidata, const ChessBoard cb, const int PatternLen);
NeighborMap NewNeighborMap();
void FreeNeighborMap(NeighborMap nbm);
void NeighborMapAddChess(NeighborMap nbm, Point p);
void NeighborMapUndo(NeighborMap nbm, Point s);
void NeighborMapFlush(NeighborMap nbm, const Stack actionHistory);
void PrintNeighborMap(NeighborMap nbm);

#endif