#include "ChessBoard.h"
#include <string.h>
const char DireNames[4] = "XYCD";
const int DireSteps[4] = {XSTEP, YSTEP, CSTEP, DSTEP};
const ChessType PlayerChessTypes[2] = {PlayerB, PlayerW};
const ChessBoardStyle ChessBoardStyle_Classic_VS = {
    "┏━", "┯━", "┓ ",
    "┠─", "┼─", "┨ ",
    "┗━", "┷━", "┛ ",
    "● ", "○ ", "◑ "};
const ChessBoardStyle ChessBoardStyle_Classic = {
    "┏ ", "┯ ", "┓ ",
    "┠ ", "┼ ", "┨ ",
    "┗ ", "┷ ", "┛ ",
    "●", "○", "◑"};

const char *GetChessSkin(ChessType type, const ChessBoardStyle style)
{
    if (type == PlayerB)
        return style[10];
    else if (type == PlayerW)
        return style[9];
    else
        return style[11];
}

ChessBoard NewChessBoard()
{
    char *re = calloc(BLEN, sizeof(char));
#ifdef DEBUG
    printfD("New ChessBoard Success\n");
#endif
    return re;
}

ChessBoard CloneChessBoard(const ChessBoard ct)
{
    char *re = malloc(BLEN * sizeof(char));
    memcpy(re, ct, BLEN);
    return re;
}

void FreeChessBoard(ChessBoard ct)
{
    free(ct);
}

int ChessBoardSave(ChessBoard ct, char *file)
{
    for (int i = 0; i < BLEN; ++i)
    {
        file[i] = ct[i] + '0';
    }
    return BLEN;
}

int ChessBoardLoad(ChessBoard *ct, char *file)
{
    *ct = NewChessBoard();
    for (int i = 0; i < BLEN; ++i)
    {
        (*ct)[i] = file[i] - '0';
    }
    return BLEN;
}

void PrintChessBoard(const ChessBoard ct, const ChessBoardStyle style)
{
    printf("   ");
    char a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf("%c ", a);
    printf("\n");
    for (int i = 0; i < LLN; i++)
    {
        printf(i < 9 ? " %d " : "%d ", i + 1);
        for (int j = 0; j < LLN; j++)
        {
            int k = GetChessXY(ct, j, i);
            if (k == BLANK)
            {
                if (i == 0)
                {
                    if (j == 0)
                        printf("%s", style[0]);
                    else if (j == LLN - 1)
                        printf("%s", style[2]);
                    else
                        printf("%s", style[1]);
                }
                else if (i == LLN - 1)
                {
                    if (j == 0)
                        printf("%s", style[6]);
                    else if (j == LLN - 1)
                        printf("%s", style[8]);
                    else
                        printf("%s", style[7]);
                }
                else
                {
                    if (j == 0)
                        printf("%s", style[3]);
                    else if (j == LLN - 1)
                        printf("%s", style[5]);
                    else
                        printf("%s", style[4]);
                }
            }
            else if (k == PlayerW)
                printf("%s", style[9]);
            else if (k == PlayerB)
                printf("%s", style[10]);
            else
                printf("%s", style[11]);
        }
        printf("%d", i + 1);
        printf("\n");
    }
    printf("   ");
    a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf("%c ", a);
    printf("\n");
}

ChessBoardInf CBINF;

ChessBoardInf NewChessBoardInf()
{
    ChessBoardInf re = malloc(sizeof(*re) * BLEN);
    for (int i = 0; i < LLN; ++i)
    {
        for (int j = 0; j < LLN; ++j)
        {
            Point p = GetPoint(i, j);

            re[p].lens[0] = LLN;
            re[p].start[0] = GetPoint(0, j);

            re[p].lens[1] = LLN;
            re[p].start[1] = GetPoint(i, 0);

            re[p].lens[2] = LLN - abs(i - j);
            re[p].start[2] = i > j ? GetPoint(i - j, 0) : GetPoint(0, j - i);

            re[p].lens[3] = LLN - abs(i + j - LLN + 1);
            re[p].start[3] = i + j > LLN - 1 ? GetPoint(LLN - 1, i + j - LLN + 1) : GetPoint(i + j, 0);
        }
    }
#ifdef DEBUG
    printfD("Get ChessBoardInf Success\n");
#endif
    return re;
}

void FreeChessBoardInf(ChessBoardInf ctn)
{
    free(ctn);
}

ChessBoardNeighbor CBNEI;

ChessBoardNeighbor NewChessBoardNeighbor()
{
    ChessBoardNeighbor re = malloc(sizeof(*re) * BLEN);
    for (int x = 0; x < LLN; ++x)
    {
        for (int y = 0; y < LLN; ++y)
        {
            int i;
            const int lb = Max(0, x - NEIGHBORRANGE), rb = Min(LLN - 1, x + NEIGHBORRANGE),
                      bb = Min(LLN - 1, y + NEIGHBORRANGE), tb = Max(0, y - NEIGHBORRANGE);
            Point p = GetPoint(x, y), t;
            re[p].len = 0;
            for (i = tb; i <= bb; i++)
            {
                if (i == y)
                    continue;
                t = GetPoint(x, i);
                re[p].neighbors[re[p].len++] = t;
            }
            for (i = -2; i <= 2; i++)
            {
                if (i == 0)
                    continue;
                if (x + i >= lb && x + i <= rb)
                {
                    t = GetPoint(x + i, y);
                    re[p].neighbors[re[p].len++] = t;
                }
                if (x + i >= lb && x + i <= rb && y + i >= tb && y + i <= bb)
                {
                    t = GetPoint(x + i, y + i);
                    re[p].neighbors[re[p].len++] = t;
                }
                if (x + i >= lb && x + i <= rb && y - i >= tb && y - i <= bb)
                {
                    t = GetPoint(x + i, y - i);
                    re[p].neighbors[re[p].len++] = t;
                }
            }
            // re[p].neighbors[re[p].len++] = p;
        }
    }
    return re;
}

void PrintPointInf(const ChessBoardInf ctinf, const Point p)
{
    printf("Point infmation of %d%c:\n", PointTo2C(p));
    for (int i = 0; i < 4; ++i)
    {
        printf("    Dir %c,\tStart = %d%c,\tLen = %d,\tStep = %d.\n", DireNames[i], PointTo2C(ctinf[p].start[i]), ctinf[p].lens[i], DireSteps[i]);
    }
}

void ChessBoardInit()
{
    static char inited = 0;
    if (inited)
        return;
    CBINF = NewChessBoardInf();
    CBNEI = NewChessBoardNeighbor();
    inited = 1;
}