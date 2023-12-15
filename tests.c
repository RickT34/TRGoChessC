#include "ACAutomaton.h"
#include "AIPlayer.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include "chessmanual.h"
#include "globals.h"
#include "surroundmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// void surroundmaptest()
// {
//     SurroundMapInit();
//     ChessBoard ct = NewChessBoard();
//     int x, y;
//     ChessType now = PlayerB;
//     while (1) {
//         while (GetInputChess(&x, &y))
//             ;
//         if (ct[GetPoint(x, y)]) {
//             SurroundMapRemoveChess(GetPoint(x, y));
//             SetChessXY(ct, x, y, BLANK);
//         } else {
//             SetChessXY(ct, x, y, now);
//             SurroundMapAddChess(GetPoint(x, y));
//         }
//         now = GetNextPlayer(now);
//         // PrintChessBoard(ct);
//         PrintSurroundMap();
//     }
// }

// void chessmanualtest()
// {
//     ChessBoard ct = NewChessBoard();
//     ChessManualInit();
//     int x, y;
//     ChessType now = PlayerB;
//     while (1) {
//         while (GetInputChess(&x, &y))
//             ;
//         SetChessXY(ct, x, y, now);
//         now = GetNextPlayer(now);
//         PrintChessBoard(ct, ChessBoardStyle_Classic);
//         Key k = ChessBoardDRowToKey(ct, 19, FULL);
//     }
// }

// void chessboardtest()
// {
//     ChessBoard ct = NewChessBoard();
//     ChessType p = PlayerB;
//     int x, y;
//     while (1) {
//         while (GetInputChess(&x, &y))
//             ;
//         system("clear");
//         SetChessXY(ct, x, y, p);
//         p = GetNextPlayer(p);
//         PrintChessBoard(ct,ChessBoardStyle_Classic);
//     }
// }

void ACAutomatontest()
{
    Trie root = NewTrie();
    char* pats[32];
    int ret[20];
    int id = 0;
    pats[id++] = "\1\0";
    pats[id++] = "\0\1";
    pats[id++] = "\0\1\1";
    pats[id++] = "\1\1\0";
    pats[id++] = "\1\1\1\0";
    pats[id++] = "\0\1\1\1";
    pats[id++] = "\0\1\1\1\1";
    pats[id++] = "\1\1\1\1\0";
    pats[id++] = "\1\1\1\1\1";
    int lens[] = { 2, 2, 3, 3, 4, 4, 5, 5, 5 };
    for (int i = 0; i < id; ++i) {
        TrieInsert(root, pats[i], lens[i], i);
    }
    TrieCompile(root);
    char s[] = "\0\0\0\1\1\1\2\1\1\0\1\1\1\1\1\0\2";
    memset(ret, 0, sizeof(ret));
    TrieQuery(s, 1, 18, root, ret);
    printArray(ret, 0, id);
    printf("\n");
    // int ret2[18][20];
    // int* ret2p[18];
    // for (int i = 0; i < 18; ++i) {
    //     ret2p[i] = &ret2[i][0];
    // }
    // memset(ret2, 0, sizeof(ret2));
    // TrieQuery2(s, 1, 18, root, ret2p);
    // for (int i = 0; i < 18; ++i) {
    //     printf("%d :", (int)s[i]);
    //     printArray(&ret2[i][0], 0, id);
    //     printf("\n");
    // }
}

// void chessboardinftest()
// {
//     ChessBoardInf ctinf = NewChessBoardInf();
//     ChessBoard ct = NewChessBoard();
//     PrintChessBoard(ct, ChessBoardStyle_Classic);
//     int x, y;
//     while (1) {
//         GetInputChess(&x, &y);
//         PrintPointInf(ctinf, GetPoint(x, y));
//     }
// }

// void chessboardNeitest()
// {
//     ChessBoardNeighbor ctn = NewChessBoardNeighbor();
//     ChessBoard ct = NewChessBoard();
//     int x, y;
//     while (1) {
//         PrintChessBoard(ct, ChessBoardStyle_Classic);
//         while (GetInputChess(&x, &y))
//             ;
//         Point p = GetPoint(x, y);
//         SetChess(ct, p, PlayerW);
//         for (int i = 0; i < ctn[p].len; ++i) {
//             SetChess(ct, ctn[p].neighbors[i], PlayerB);
//         }
//     }
// }
