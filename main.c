#include "ACAutomaton.h"
#include "chessmanual.h"
#include "surroundmap.h"
#include "globals.h"

void surroundmaptest()
{
    SurroundMapInit();
    ChessTable ct = NewChessTable();
    int x, y;
    Player now = PlayerB;
    while (1) {
        while (GetInputChess(&x, &y))
            ;
        if (ct[GetPoint(x, y)]) {
            SurroundMapRemoveChess(GetPoint(x, y));
            SetChessXY(ct, x, y, BLANK);
        } else {
            SetChessXY(ct, x, y, now);
            SurroundMapAddChess(GetPoint(x, y));
        }
        now = GetNextPlayer(now);
        // PrintChessTable(ct);
        PrintSurroundMap();
    }
}

void chessmanualtest()
{
    ChessTable ct = NewChessTable();
    ChessManualInit();
    int x, y;
    Player now = PlayerB;
    while (1) {
        while (GetInputChess(&x, &y))
            ;
        SetChessXY(ct, x, y, now);
        now = GetNextPlayer(now);
        PrintChessTable(ct);
        Key k = ChessTableDRowToKey(ct, 19, FULL);
    }
}

void chesstabletest()
{
    ChessTable ct = NewChessTable();
    Player p = PlayerB;
    int x, y;
    while (1) {
        while (GetInputChess(&x, &y))
            ;
        system("clear");
        SetChessXY(ct, x, y, p);
        p = GetNextPlayer(p);
        PrintChessTable(ct);
    }
}

void ACAutomatontest()
{
    Trie root = NewTrie();
    char* pats[32];
    int ret[20];
    int id=0;
    pats[id++] = "\1\0";
    pats[id++] = "\0\1";
    pats[id++] = "\0\1\1";
    pats[id++] = "\1\1\0";
    pats[id++] = "\1\1\1\0";
    pats[id++] = "\0\1\1\1";
    pats[id++] = "\0\1\1\1\1";
    pats[id++] = "\1\1\1\1\0";
    pats[id++] = "\1\1\1\1\1";
    int lens[]={2,2,3,3,4,4,5,5,5};
    for(int i=0;i<id;++i){
        TrieInsert(root, pats[i], lens[i], i);
    }
    TrieCompile(root);
    char s[]="\0\0\0\1\1\1\2\1\1\0\1\1\1\1\1\0\2";
    TrieQuery(s, 1, 18, root, ret);
    printArray(ret, 0, id);
}

int main(int args, char** argv)
{
    ACAutomatontest();
    return 0;
}