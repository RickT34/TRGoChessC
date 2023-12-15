#ifndef _GameManagerH
#define _GameManagerH
#include "ACAutomaton.h"
#include "Player.h"
#include "Stack.h"
#include "chessboard.h"
#include "globals.h"

typedef struct {
    Point point;
    int playerid;
} *Action,Action_Raw;

typedef enum {
    GameStatus_Begin,
    GameStatus_Gaming,
    GameStatus_End,
    GameStatus_Pause=(1<<4)
} GameStatus;
#define IsGameStopped(game) (game->status & GameStatus_Pause) || game->status == GameStatus_End

typedef struct {
    ChessBoard chessboard;
    Stack history;
    Player players[2];
    int nowPlayerID;
    GameStatus status;
} * Game;
#define GameNextPlayerID(player) ((player) ^ 1)
#define GamePrePlayerID(player) ((player) ^ 1)
Player GameGetNextPlayer(Game game);
Player GameGetNowPlayer(Game game);
Game NewGame(Player player1, Player player2);
int PauseGame(Game game);
int ContinueGame(Game game);
void GameManagerInit();
void FreeGame(Game game);
int GameNextTurn(Game game);
int GameUndo(Game game);
int GameSave(Game game, char* file);
int GameLoad(Game* data, char* file);
const extern ChessType PlayerChessTypes[2];

typedef struct{
    Action_Raw data[BLEN];
    int datalen;
} *GameRecord;
GameRecord NewGameRecord(const Game game);
void FreeGameRecord(GameRecord gr);
int GameRecordSave(const GameRecord gr, char* file);
int GameRecordLoad(GameRecord* gr, char* file);
ChessBoard GameRecordRead(const GameRecord gr, const int flame);
#endif