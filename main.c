#include "surroundmap.h"
#include "chessmanual.h"

void surroundmaptest(){
    SurroundMapInit();
    ChessTable ct=NewChessTable();
    int x,y;
    Player now=PlayerB;
    while(1){
        while(GetInputChess(&x, &y));
        if(ct[GetPoint(x,y)]){
            SurroundMapRemoveChess(GetPoint(x, y));
            SetChessXY(ct,x,y,BLANK);
            }
        else{  
        SetChessXY(ct, x, y, now);
        SurroundMapAddChess(GetPoint(x, y));
        }
        now=GetNextPlayer(now);
        //PrintChessTable(ct);
        PrintSurroundMap();
    }
}

void chessmanualtest(){
    ChessTable ct=NewChessTable();
    ChessManualInit();
    int x,y;
    Player now=PlayerB;
    while(1){
        while(GetInputChess(&x, &y));
        SetChessXY(ct, x, y, now);
        now=GetNextPlayer(now);
        PrintChessTable(ct);
        Key k = ChessTableDRowToKey(ct, 19, FULL);
    }
}

int main(int args, char** argv){
    surroundmaptest();
    return 0;
}