#include"Train.h"

int TrainGetResult(Game game){
    printfD("Get result...\n");
    while(game->status!=GameStatus_End){
        GameNextTurn(game);
        PrintChessBoard(game->chessboard,ChessBoardStyle_Classic);
    }
    return game->nowPlayerID;
}

int TrainTestAI(const Power* power1, const Power* power2){
    Player ai0=NewAIPlayer("ai0",0,power1);
    Player ai1=NewAIPlayer("ai0",1,power1);
    Game game=NewGame(ai0,ai1);
    int re=TrainGetResult(game);
    FreeGame(game);
    return re;
}

