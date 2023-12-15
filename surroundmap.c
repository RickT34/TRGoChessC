#include "surroundmap.h"

#define push(point) SurroundList[SurroundListCount++] = point;
#define pop() SurroundList[--SurroundListCount]

#define addoneAndCheck(point) \
    if (existmap[point] == 0) \
        push(point);          \
    existmap[point] += 1

static ChessBoard existmap;
Point SurroundList[BLEN];
int SurroundListCount = 0;

void SurroundMapInit()
{
    existmap = NewChessBoard();
}

void Clean()
{
    int iv = 0, i = 0;
    while (iv < SurroundListCount) {
        if (existmap[SurroundList[iv]] == 0 || existmap[SurroundList[iv]] >= 64) {
            iv += 1;
        } else {
            SurroundList[i] = SurroundList[iv];
            i += 1;
            iv += 1;
        }
    }
    SurroundListCount = i;
#ifdef DEBUG
    printfD("Cleaned %d", iv - i);
    printPointList(SurroundList, SurroundListCount);
#endif
}

void SurroundMapAddChess(Point point)
{
    existmap[point] += 64;
    int x, y, i;
    Point t;
    PointToXY(point, x, y);
    int lb = Max(0, x - ADDRANGE), rb = Min(LLN - 1, x + ADDRANGE),
        bb = Min(LLN - 1, y + ADDRANGE), tb = Max(0, y - ADDRANGE);
#ifdef DEBUG
    printPoint(point, "Centre: ");
    printfD("lb: %d, rb: %d, bb: %d, tb: %d", lb, rb, bb, tb);
#endif
    for (i = tb; i <= bb; i++) {
        if (i == y)
            continue;
        t = GetPoint(x, i);
        addoneAndCheck(t);
    }
    for (i = -2; i <= 2; i++) {
        if (i == 0)
            continue;
        if (x + i >= lb && x + i <= rb) {
            t = GetPoint(x + i, y);
            addoneAndCheck(t);
        }
        if (x + i >= lb && x + i <= rb && y + i >= tb && y + i <= bb) {
            t = GetPoint(x + i, y + i);
            addoneAndCheck(t);
        }
        if (x + i >= lb && x + i <= rb && y - i >= tb && y - i <= bb) {
            t = GetPoint(x + i, y - i);
            addoneAndCheck(t);
        }
    }
    Clean();
}
void SurroundMapRemoveChess(Point point)
{
    existmap[point] -= 64;
    if (existmap[point] > 0)
        push(point);
    int x, y, i;
    Point t;
    PointToXY(point, x, y);
    int lb = Max(0, x - ADDRANGE), rb = Min(LLN - 1, x + ADDRANGE),
        bb = Min(LLN - 1, y + ADDRANGE), tb = Max(0, y - ADDRANGE);
#ifdef DEBUG
    printPoint(point, "Centre: ");
    printfD("lb: %d, rb: %d, bb: %d, tb: %d", lb, rb, bb, tb);
#endif
    for (i = tb; i <= bb; i++) {
        if (i == y)
            continue;
        t = GetPoint(x, i);
        existmap[t] -= 1;
    }
    for (i = -2; i <= 2; i++) {
        if (i == 0)
            continue;
        if (x + i >= lb && x + i <= rb) {
            t = GetPoint(x + i, y);
            existmap[t] -= 1;
        }
        if (x + i >= lb && x + i <= rb && y + i >= tb && y + i <= bb) {
            t = GetPoint(x + i, y + i);
            existmap[t] -= 1;
        }
        if (x + i >= lb && x + i <= rb && y - i >= tb && y - i <= bb) {
            t = GetPoint(x + i, y - i);
            existmap[t] -= 1;
        }
    }
    Clean();
}

void PrintSurroundMap()
{
    printf("   ");
    char a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf(" %c ", a);
    printf("\n");
    for (int i = 0; i < LLN; i++) {
        printf(i < 9 ? " %d " : "%d ", i + 1);
        for (int j = 0; j < LLN; j++) {
            int k = GetChessXY(existmap, j, i);
            printf(k == 0 ? " . " : "%2d ", k);
        }
        printf("%d", i + 1);
        printf("\n");
    }
    printf("   ");
    a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf(" %c ", a);
    printf("\n");
}