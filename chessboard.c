#include "chessboard.h"

ChessTable NewChessTable()
{
    char* re = malloc(sizeof(char) * BLEN);
    for (int i = 0; i < BLEN; i++)
        re[i] = 0;
#ifdef DEBUG
    printfD("New Board Success\n");
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
                        printf("┏━");
                    else if (j == LLN - 1)
                        printf("┓ ");
                    else
                        printf("┯━");
                } else if (i == LLN - 1) {
                    if (j == 0)
                        printf("┗━");
                    else if (j == LLN - 1)
                        printf("┛ ");
                    else
                        printf("┷━");
                } else {
                    if (j == 0)
                        printf("┠─");
                    else if (j == LLN - 1)
                        printf("┨ ");
                    else
                        printf("┼─");
                }
            } else if (k == PlayerW)
                printf("● ");
            else if (k == PlayerB)
                printf("○ ");
            else
                printf("◑ ");
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