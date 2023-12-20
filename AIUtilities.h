#ifndef _AIUtilitiesH
#define _AIUtilitiesH

#include "ACAutomaton.h"
#include "ChessPot.h"
#include "Stack.h"
#include "ChessBoard.h"
#include "Globals.h"

#define POWERSLEN ((LLN * 3 - 1) * 2)

typedef struct {
    Power* linePower[BLEN][4];
    Power powers[POWERSLEN];
    Power powerSum;
    char needflush;
} * PowerMap;

typedef struct {
    char map[BLEN];
    ChessPot pot;
    Point history[BLEN];
    int historyCount;
    char needflush;
} * NeighborMap;

typedef struct {
    PowerMap powerMap;
    NeighborMap neighborMap;
    Trie patterns;
    Power* patternPowers;
    int playerid;
} * AIData;

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