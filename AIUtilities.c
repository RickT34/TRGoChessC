

#include "Action.h"
#include "AIUtilities.h"
#include <string.h>


PowerMap NewPowerMap()
{
    PowerMap re = calloc(1, sizeof(*re));
    int pl = 0;
    for (int x = 0; x < LLN; ++x) {
        for (int y = 0; y < LLN; ++y) {
            Point p = GetPoint(x, y);
            for (int d = 0; d < DireLen; ++d) {
                Point s = CBINF[p].start[d];
                if (re->linePower[s][d] == NULL) {
                    re->linePower[s][d] = &re->powers[pl++];
                }
                re->linePower[p][d] = re->linePower[s][d];
            }
        }
    }
    return re;
}

void PrintPowerMap(PowerMap pm)
{
    for (int d = 0; d < DireLen; ++d) {
        printf("%c:\n", DireNames[d]);
        for (int x = 0; x < LLN; ++x) {
            for (int y = 0; y < LLN; ++y) {
                printf("%d ", *(pm->linePower)[GetPoint(y, x)][d]);
            }
            putchar('\n');
        }
    }
    printf("Sum: %d\n", pm->powerSum);
}

NeighborMap NewNeighborMap()
{
    NeighborMap re = malloc(sizeof(*re));
    memset(re->map, 0, sizeof(re->map));
    re->pot = NewChessPot();
    re->history = NewStack(BLEN);
    re->needflush = 0;
    return re;
}

void NeighborMapAddChess(NeighborMap nbm, Point p)
{
    ChessBoardNeighbor cbn = &CBNEI[p];
    ChessBoard cb = nbm->map;
    ChessPot pot = nbm->pot;
    Point nxt = pot->nodes[ChessPotHead].nxt;
    StackPush(nbm->history, (void*)nxt);
    for (int i = 0; i < cbn->len; ++i) {
        Point p = cbn->neighbors[i];
        if (cb[p] == 0) {
            ChessPotTie(pot, p, nxt);
            nxt = p;
        }
        cb[p] += 1;
    }
    ChessPotTie(pot, ChessPotHead, nxt);
}

void NeighborMapUndo(NeighborMap nbm, Point s)
{
    ChessBoard cb = nbm->map;
    ChessPot pot = nbm->pot;
    Point p = (Point)StackPop(nbm->history);
    ChessPotTie(nbm->pot, ChessPotHead, p);
    ChessBoardNeighbor cbn = &CBNEI[s];
    for (int i = 0; i < cbn->len; ++i) {
        Point p = cbn->neighbors[i];
        cb[p] -= 1;
    }
}

void NeighborMapFlush(NeighborMap nbm, const Stack actionHistory)
{
    StackClean(nbm->history);
    ChessPotClean(nbm->pot);
    memset(nbm->map, 0, sizeof(nbm->map));
    nbm->needflush = 0;
    for (int i = 0; i < actionHistory->Count; ++i) {
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
    for (int i = 0; i < LLN; i++) {
        printf(i < 9 ? " %d " : "%d ", i + 1);
        for (int j = 0; j < LLN; j++) {
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
    for (Point p = pot->nodes[ChessPotHead].nxt; p != ChessPotTail; p = pot->nodes[p].nxt) {
        printf("%d%c, ", PointTo2C(p));
    }
    putchar('\n');
}

void NeighborMaptest()
{
    char buff[64];
    NeighborMap nbm = NewNeighborMap();
    while (1) {
        PrintNeighborMap(nbm);
        int x, y;
        scanf("%d%d", &x, &y);
        if (x < 0)
            NeighborMapUndo(nbm, GetPoint(-x, y));
        else
            NeighborMapAddChess(nbm, GetPoint(x, y));
    }
}

void PowerMaptest()
{
    char buff[64];
    PowerMap pm = NewPowerMap();
    while (1) {
        PrintPowerMap(pm);
        int x, y;
        scanf("%d%d", &x, &y);
        for (int d = 0; d < DireLen; ++d) {
            SETPower(pm, GetPoint(x, y), d, d + 1);
        }
    }
}