#include "AIUtilities.h"
#include "Action.h"
#include <string.h>
#include <assert.h>
#include "mt19937.h"

PowerMap NewPowerMap()
{
    PowerMap re = malloc(sizeof(*re));
    memset(re->linePower, 0, sizeof(re->linePower));
    int pl = 0;
    for (int x = 0; x < LLN; ++x)
    {
        for (int y = 0; y < LLN; ++y)
        {
            Point p = GetPoint(x, y);
            for (int d = 0; d < DireLen; ++d)
            {
                Point s = CBINF[p].start[d];
                if (re->linePower[s][d] == NULL)
                {
                    re->powers[pl] = 0;
                    re->linePower[s][d] = &re->powers[pl++];
                }
                re->linePower[p][d] = re->linePower[s][d];
            }
        }
    }
    #ifdef DEBUG
    assert(pl == POWERSLEN);
    #endif
    return re;
}

void FreePowerMap(PowerMap pm)
{
    free(pm);
}

void PowerMapFlush(AIData aidata, const ChessBoard cb, const int PatternLen)
{
    PowerMap pm = aidata->powerMap;
    memset(pm->powers, 0, sizeof(pm->powers));
    pm->powerSum = 0;

    char computed[POWERSLEN];
    memset(computed, 0, sizeof(computed));

    for (int x = 0; x < LLN; ++x)
    {
        for (int y = 0; y < LLN; ++y)
        {
            Point p = GetPoint(x, y);
            for (int d = 0; d < DireLen; ++d)
            {
                int index = pm->linePower[p][d] - pm->powers;
                if (computed[index])
                    continue;
                computed[index] = 1;
                Power ls = TrieQuery(&GetChess(cb, CBINF[p].start[d]),
                                     DireSteps[d], CBINF[p].lens[d], aidata->patterns);
                *(pm->linePower[p][d]) = ls;
                pm->powerSum += ls;
            }
        }
    }
}

void PrintPowerMap(PowerMap pm)
{
    for (int d = 0; d < DireLen; ++d)
    {
        printf("%c:\n", DireNames[d]);
        for (int x = 0; x < LLN; ++x)
        {
            for (int y = 0; y < LLN; ++y)
            {
                printf("%f ", *(pm->linePower)[GetPoint(y, x)][d]);
            }
            putchar('\n');
        }
    }
    printf("Sum: %f\n", pm->powerSum);
}

PowerMap ClonePowerMap(const PowerMap pm)
{
    PowerMap re = NewPowerMap();
    memcpy(re->powers, pm->powers, sizeof(re->powers));
    re->powerSum = pm->powerSum;
    return re;
}

ChessPot NewChessPot()
{
    ChessPot re = malloc(sizeof(*re));
    ChessPotTie(re, ChessPotHead, ChessPotTail);
    return re;
}

ChessPot CloneChessPot(const ChessPot cp)
{
    ChessPot re = malloc(sizeof(*re));
    memcpy(re, cp, sizeof(*cp));
    return re;
}

void FreeChessPot(ChessPot pot)
{
    free(pot);
}

NeighborMap NewNeighborMap()
{
    NeighborMap re = malloc(sizeof(*re));
    memset(re->map, 0, sizeof(re->map));
    re->pot = NewChessPot();
    re->historyCount = 0;
    return re;
}

NeighborMap CloneNeighborMap(const NeighborMap nbm)
{
    NeighborMap re = malloc(sizeof(*re));
    memcpy(re, nbm, sizeof(*nbm));
    re->pot = CloneChessPot(nbm->pot);
    return re;
}

void FreeNeighborMap(NeighborMap nbm)
{
    FreeChessPot(nbm->pot);
    free(nbm);
}

void NeighborMapAddChess(NeighborMap nbm, Point p)
{
    ChessBoardNeighbor cbn = &CBNEI[p];
    char *cb = nbm->map;
    ChessPot pot = nbm->pot;
    Point nxt = pot->nxtnode[ChessPotHead];
    nbm->history[nbm->historyCount++] = nxt;
    for (int i = 0; i < cbn->len; ++i)
    {
        Point p = cbn->neighbors[i];
        if (cb[p] == 0)
        {
            ChessPotTie(pot, p, nxt);
            nxt = p;
        }
        cb[p] += 1;
    }
    ChessPotTie(pot, ChessPotHead, nxt);
}

void NeighborMapUndo(NeighborMap nbm, Point s)
{
    char *cb = nbm->map;
    ChessPot pot = nbm->pot;
    Point p = nbm->history[--nbm->historyCount];
    ChessPotTie(nbm->pot, ChessPotHead, p);
    ChessBoardNeighbor cbn = &CBNEI[s];
    for (int i = 0; i < cbn->len; ++i)
    {
        Point p = cbn->neighbors[i];
        cb[p] -= 1;
    }
}

void NeighborMapFlush(NeighborMap nbm, const Stack actionHistory)
{
    nbm->historyCount = 0;
    ChessPotClean(nbm->pot);
    memset(nbm->map, 0, sizeof(nbm->map));
    for (int i = 0; i < actionHistory->Count; ++i)
    {
        NeighborMapAddChess(nbm, ((Action)actionHistory->Items[i])->point);
    }
}

void PrintNeighborMap(NeighborMap nbm)
{
    printf("   ");
    char a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf(" %c ", a);
    printf("\n");
    for (int i = 0; i < LLN; i++)
    {
        printf(i < 9 ? " %d " : "%d ", i + 1);
        for (int j = 0; j < LLN; j++)
        {
            int k = GetChessXY(nbm->map, j, i);
            printf(k == 0 ? " . " : "%2d ", k);
        }
        printf("%d", i + 1);
        printf("\n");
    }
    printf("   ");
    a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf(" %c ", a);
    printf("\nPot: ");

    ChessPot pot = nbm->pot;
    int count = 0;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        printf("%d%c, ", PointTo2C(p));
        count += 1;
        if (count > BLEN)
        {
            assert(0);
        }
    }
    printf("Count: %d\n", count);
}

void NeighborMaptest()
{
    char buff[64];
    NeighborMap nbm = NewNeighborMap();
    while (1)
    {
        PrintNeighborMap(nbm);
        int x, y;
        scanf("%d%d", &x, &y);
        if (x < 0)
            NeighborMapUndo(nbm, GetPoint(-x, y));
        else
            NeighborMapAddChess(nbm, GetPoint(x, y));
    }
}
#define SETPower(pm, point, d, power) pm->powerSum += power - *(pm->linePower)[point][d], *(pm->linePower)[point][d] = power
void PowerMaptest()
{
    char buff[64];
    PowerMap pm = NewPowerMap();
    while (1)
    {
        PrintPowerMap(pm);
        int x, y;
        scanf("%d%d", &x, &y);
        for (int d = 0; d < DireLen; ++d)
        {
            SETPower(pm, GetPoint(x, y), d, d + 1);
        }
    }
}

uint64 TurnTable[2][BLEN];

ZobristTable NewZobristTable()
{
    static char inited = 0;
    if (!inited)
    {
        for (int p = 0; p < 2; ++p)
        {
            init_genrand64(3213123442ull ^ p);
            for (int i = 0; i < BLEN; ++i)
            {
                TurnTable[p][i] = genrand64_int64();
            }
        }
        inited = 1;
    }
    ZobristTable re = malloc(sizeof(*re));
    re->turnTable[0] = &TurnTable[0][0];
    re->turnTable[1] = &TurnTable[1][0];
    memset(re->hashTable, 0, sizeof(re->hashTable));
    re->count = 0;
    re->start = genrand64_int64();
    return re;
}
ZobristTable CloneZobristTable(const ZobristTable zb)
{ // not clone hashtable
    ZobristTable re = malloc(sizeof(*re));
    memset(re->hashTable, 0, sizeof(re->hashTable));
    re->count = 0;
    re->start = zb->start;
    re->turnTable[0] = zb->turnTable[0];
    re->turnTable[1] = zb->turnTable[1];
    return re;
}

void FreeZobristTable(ZobristTable zt)
{
    free(zt);
}

HashNode ZobristTableFind(ZobristTable zt, const uint64 key)
{
    HashNode node = zt->hashTable[key & HASHMASK];
    while (node != NULL && node->val.key != key)
    {
        node = node->nxt;
    }
    return node;
}

void ZobristTableInsert(ZobristTable zt, const uint64 key, const Power power)
{
    HashNode node = &zt->nodesPool[zt->count++];
    node->val.key = key;
    node->val.power = power;
    int k = key & HASHMASK;
    node->nxt = zt->hashTable[k];
    zt->hashTable[k] = node;
#ifdef DEBUG
    assert(zt->count <= HASHPOOLLEN);
#endif
}

AIData CloneAIData(const AIData aidata) // Not Clone TRIE, Zobrist
{
    AIData re = malloc(sizeof(*re));
    re->powerMap = ClonePowerMap(aidata->powerMap);
    re->neighborMap = CloneNeighborMap(aidata->neighborMap);
    re->patterns = aidata->patterns;
    re->playerid = aidata->playerid;
    re->needflush = aidata->needflush;
    re->zobristTable = CloneZobristTable(aidata->zobristTable);

    return re;
}

void PrintAIData(const AIData aidata)
{
    printf("PowerMap:\n");
    PrintPowerMap(aidata->powerMap);
    printf("NeighborMap:\n");
    PrintNeighborMap(aidata->neighborMap);
    printf("Playerid: %d, needflush: %d\n", aidata->playerid, aidata->needflush);
}

void FreeAIData(AIData aidata)
{
    FreePowerMap(aidata->powerMap);
    FreeNeighborMap(aidata->neighborMap);
    FreeZobristTable(aidata->zobristTable);
    free(aidata);
}