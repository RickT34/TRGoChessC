#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void MakeUI(Game game)
{
    // system("clear");
    printf("Round: %d\n", game->history->Count);
    PrintChessBoard(game->chessboard, ChessBoardStyle_Classic);
    printf("Next: ");
    PrintPlayer(game, GameNextPlayerID(game->nowPlayerID));
    putchar('\n');
}
void MakeProcessBar(int p, int len,const int width){// p in [1,len]
    putchar('[');
    char last='[';
    for(int i=1;i<=width;++i){
        if(i*len<p*width)putchar(last='=');
        else {
            if(last=='=')putchar(last='>');
            else
            putchar('-');
        }
    }
    putchar(']');
    printf(" (%d/%d)\n",p,len);
}

void StartGameRecord(Game game)
{
    char buff[64];
    buff[0]=0;
    ChessBoard re;
    int flame=game->history->Count;
    GameRecord gr=NewGameRecord(game);
    do{
        re=GameRecordRead(gr, flame-1);
        if(re!=NULL){
            printf("Recode Flame: %d\n",flame);
            PrintChessBoard(re, ChessBoardStyle_Classic);
            MakeProcessBar(flame, gr->datalen, 30);
            FreeChessBoard(re);
            re=NULL;
        }
        Input(buff, 64);
        sscanf(buff,"%d", &flame);
    } while (buff[0] != 'q');
}



Point GetPointInput(const Game game, const char* s)
{
    int x, y;
    char c;
    if (sscanf(s, "%d%c", &y, &c) != 2)
        if (sscanf(s, "%c%d", &c, &y) != 2)
            return PointNULL;
    x = c - 'a';
    y -= 1;
    if (!IsLegalXY(x, y))
        return PointNULL;
    if (GetChessXY(game->chessboard, x, y) != BLANK)
        return PointNULL;
    return GetPoint(x, y);
}

int InputCommamd(Game game, char* buff){
    if (buff[0] == '-') {
        GameUndo(game);
        if(GameGetNextPlayer(game)->type==PlayerType_AI)GameUndo(game);
        return 0;
    } else if (buff[0] == '>') {
        char file[2048];
        int l = GameSave(game, file);
        file[l] = 0;
        puts(file);
        return 0;
    } else if (buff[0] == 'r') {
        StartGameRecord(game);
    } else if (buff[0] == '<') {
        Game loadgame;
        GameLoad(&loadgame, buff + 1);
        Start(loadgame);
        FreeGame(game);
        return 0;
    }
    return 1;
}

void PrintPlayer(Game game, int id){
    printf("%s %s",GetChessSkin(PlayerChessTypes[id], ChessBoardStyle_Classic),
    game->players[id]->name);
}
char buff[4096];
#define BUFFSIZE 4096
void Start(Game game)
{
    int ret;
    do {
        MakeUI(game);
        if (IsGameStopped(game)) {
            if(game->status==GameStatus_End)
                {
                    PrintPlayer(game, game->nowPlayerID);
                    printf(" Win!\n");
                    }
            printf("Game stopped!\n");
            Input(buff, BUFFSIZE);
            InputCommamd(game, buff);
        }
        else{
            if (GameGetNextPlayer(game)->type == PlayerType_Human) {
                Point p = PointNULL;
                while (p == PointNULL) {
                    Input(buff, BUFFSIZE);
                    if (InputCommamd(game, buff)) {
                        p = GetPointInput(game, buff);
                    }
                    else{
                        MakeUI(game);
                    }
                }
                *(Point*)GameGetNextPlayer(game)->data = p;
            }
            ret = GameNextTurn(game);
            PrintPlayer(game, game->nowPlayerID);
            printf(" on %d%c\n",PointTo2C(((Action)StackTop(game->history))->point));
        }
    } while (1);
    
}



int main(int args, char** argv)
{
    GATest();
    Input(buff, BUFFSIZE);
    ChessBoardInit();
    GameManagerInit();
    
    // TrainTestAI(AIPatternPowers_Default,AIPatternPowers_Default);
    // ChessPotTest();
    // NeighborMaptest();
    // PowerMaptest();
    Player p1 = NewHumanPlayer("P1"), p2 = NewAIPlayer("AI", 1, AIPatternPowers_Default);
    Game game = NewGame(p1, p2);
    Start(game);
    return 0;
}