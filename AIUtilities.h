#ifndef _AIUtilitiesH
#define _AIUtilitiesH

#include "ACAutomaton.h"
#include "ChessPot.h"
#include "Stack.h"
#include "ChessBoard.h"
#include "Globals.h"

#define POWERSLEN ((LLN * 3 - 1) * 2)

typedef struct
{
    Power *linePower[BLEN][4];
    Power powers[POWERSLEN];
    Power powerSum;
} *PowerMap;

typedef struct
{
    char map[BLEN];
    ChessPot pot;
    Point history[BLEN];
    int historyCount;
} *NeighborMap;

#define HASHLEN 1000003
#define HASHSTEP 13
typedef struct
{
    uint64 turnTable[2][BLEN];
    uint64 start;
    uint64 hashTable[HASHLEN];
} *ZobristTable;
ZobristTable NewZobristTable();
int ZobristTableFindAndInsert(ZobristTable zt, const uint64 key);
void FreeZobristTable(ZobristTable zt);
#define ZobristNextKey(zt, key, point, player) ((key) ^ (zt->turnTable[player][point]))
#define ZobristClean(zt) memset(zt->hashTable, 0, sizeof(zt->hashTable))

typedef struct
{
    PowerMap powerMap;
    NeighborMap neighborMap;
    Trie patterns;
    Power *patternPowers;
    ZobristTable zobristTable;
    int playerid;
    char needflush;
} *AIData;

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