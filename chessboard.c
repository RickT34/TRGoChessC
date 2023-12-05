#include "chessboard.h"
const char DireNames[4] = "XYCD";
const int DireSteps[4] = { XSTEP, YSTEP, CSTEP, DSTEP };
const char* ChessTableStyle[12] = {
    "┏━", "┯━", "┓ ",
    "┠─", "┼─", "┨ ",
    "┗━", "┷━", "┛ ",
    "● ", "○ ", "◑ "
};

ChessTable NewChessTable()
{
    char* re = malloc(sizeof(char) * BLEN);
    for (int i = 0; i < BLEN; i++)
        re[i] = 0;
#ifdef DEBUG
    printfD("New ChessTable Success\n");
#endif
    return re;
}

void PrintChessTable(ChessTable ct)
{
    printf("   ");
    char a = 'a';
    for (int i = 0; i < LLN; i++, a++)
        printf("%c ", a);
    printf("\n");
    for (int i = 0; i < LLN; i++) {
        printf(i < 9 ? " %d " : "%d ", i + 1);
        for (int j = 0; j < LLN; j++) {
            int k = GetChessXY(ct, j, i);
            if (k == 0) {
                if (i == 0) {
                    if (j == 0)
                        printf("%s", ChessTableStyle[0]);
                    else if (j == LLN - 1)
                        printf("%s", ChessTableStyle[2]);
                    else
                        printf("%s", ChessTableStyle[1]);
                } else if (i == LLN - 1) {
                    if (j == 0)
                        printf("%s", ChessTableStyle[6]);
                    else if (j == LLN - 1)
                        printf("%s", ChessTableStyle[8]);
                    else
                        printf("%s", ChessTableStyle[7]);
                } else {
                    if (j == 0)
                        printf("%s", ChessTableStyle[3]);
                    else if (j == LLN - 1)
                        printf("%s", ChessTableStyle[5]);
                    else
                        printf("%s", ChessTableStyle[4]);
                }
            } else if (k == PlayerW)
                printf("%s", ChessTableStyle[9]);
            else if (k == PlayerB)
                printf("%s", ChessTableStyle[10]);
            else
                printf("%s", ChessTableStyle[11]);
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

int GetInputChess(int* x, int* y)
{
    char c;
    if (scanf("%d%c", y, &c) != 2)
        if (scanf("%c%d", &c, y) != 2)
            return 1;
    *x = c - 'a';
    *y -= 1;
    return !IsLegalXY(*x, *y);
}

ChessTableInf GetChessTableInf()
{
    ChessTableInf re = malloc(sizeof(*re) * BLEN);
    for (int i = 0; i < LLN; ++i) {
        for (int j = 0; j < LLN; ++j) {
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
    printfD("Get ChessTableInf Success\n");
#endif
    return re;
}

void PrintPointInf(ChessTableInf ctinf, Point p)
{
    printf("Point infmation of %d%c:\n", PointTo2C(p));
    for (int i = 0; i < 4; ++i) {
        printf("    Dir %c,\tStart = %d%c,\tLen = %d,\tStep = %d.\n", DireNames[i], PointTo2C(ctinf[p].start[i]), ctinf[p].lens[i], DireSteps[i]);
    }
}