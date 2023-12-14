#include "ACAutomaton.h"
#include "AIPlayer.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include "chessmanual.h"
#include "globals.h"
#include "surroundmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// void surroundmaptest()
// {
//     SurroundMapInit();
//     ChessTable ct = NewChessTable();
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
//         // PrintChessTable(ct);
//         PrintSurroundMap();
//     }
// }

// void chessmanualtest()
// {
//     ChessTable ct = NewChessTable();
//     ChessManualInit();
//     int x, y;
//     ChessType now = PlayerB;
//     while (1) {
//         while (GetInputChess(&x, &y))
//             ;
//         SetChessXY(ct, x, y, now);
//         now = GetNextPlayer(now);
//         PrintChessTable(ct, ChessTableStyle_Classic);
//         Key k = ChessTableDRowToKey(ct, 19, FULL);
//     }
// }

// void chesstabletest()
// {
//     ChessTable ct = NewChessTable();
//     ChessType p = PlayerB;
//     int x, y;
//     while (1) {
//         while (GetInputChess(&x, &y))
//             ;
//         system("clear");
//         SetChessXY(ct, x, y, p);
//         p = GetNextPlayer(p);
//         PrintChessTable(ct,ChessTableStyle_Classic);
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

// void chesstableinftest()
// {
//     ChessTableInf ctinf = NewChessTableInf();
//     ChessTable ct = NewChessTable();
//     PrintChessTable(ct, ChessTableStyle_Classic);
//     int x, y;
//     while (1) {
//         GetInputChess(&x, &y);
//         PrintPointInf(ctinf, GetPoint(x, y));
//     }
// }

// void chesstableNeitest()
// {
//     ChessTableNeighbor ctn = NewChessTableNeighbor();
//     ChessTable ct = NewChessTable();
//     int x, y;
//     while (1) {
//         PrintChessTable(ct, ChessTableStyle_Classic);
//         while (GetInputChess(&x, &y))
//             ;
//         Point p = GetPoint(x, y);
//         SetChess(ct, p, PlayerW);
//         for (int i = 0; i < ctn[p].len; ++i) {
//             SetChess(ct, ctn[p].neighbors[i], PlayerB);
//         }
//     }
// }
void MakeUI(Game game)
{
    // system("clear");
    printf("Round: %d\n",game->history->Count);
    PrintChessTable(game->chesstable, ChessTableStyle_Classic);
    printf("Now: %s -- %s\n",
    GetChessSkin(PlayerChessTypes[NextPlayerID(game->nowPlayerID)], ChessTableStyle_Classic),
    GameGetNowPlayer(game)->name);

}
Point GetInput(Game game)
{
    Point re=PointNULL;
    while(re==PointNULL){
        printf(">:");
        char buff[16];
        fgets(buff, 16, stdin);
        if (buff[0] == '-') {
            GameUndo(game);
            MakeUI(game);
            continue;
        }
        if(buff[0]=='>'){
            char file[1024];
            int l = GameSave(game, file);
            file[l]=0;
            puts(file);
            continue;
        }
        int x, y;
        char c;
        if (sscanf(buff,"%d%c", &y, &c) != 2)
            if (sscanf(buff,"%c%d", &c, &y) != 2)
                continue;
        x = c - 'a';
        y -= 1;
        if(!IsLegalXY(x, y))continue;
        if (GetChessXY(game->chesstable, x, y) != BLANK)continue;
        re=GetPoint(x, y);
    }
    return re;
}



void Start()
{
    Player p1 = NewHumanPlayer("P1"), p2 = NewHumanPlayer("P2");
    Game game = NewGame(p1, p2);
    int ret;
    do {
        MakeUI(game);
        if(GameGetNextPlayer(game)->type==PlayerType_Human){
            Point p=GetInput(game);
            *(Point*)GameGetNextPlayer(game)->data=p;
        }
    } while ((ret = GameNextTurn(game)) == 0);
    MakeUI(game);
    if (ret == 1) {
        printf("%s Win!\n", game->players[~game->nowPlayerID]->name);
    }
}

int main(int args, char** argv)
{
    Start();
    return 0;
}