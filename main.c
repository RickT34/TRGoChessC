#include "surroundmap.h"


int main(int args, char** argv){
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