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
} * Action;

typedef struct {
    ChessTable chesstable;
    Stack history;
    Player players[2];
    int nowPlayerID;
    ChessTableInf chesstableInf;
    Trie winPattern;
} * Game;
#define NextPlayerID(player) ((player) ^ 1)
#define PrePlayerID(player) ((player) ^ 1)
Player GameGetNextPlayer(Game game);
Player GameGetNowPlayer(Game game);
Game NewGame(Player player1, Player player2);
void FreeGame(Game game);
int GameNextTurn(Game game);
int GameUndo(Game game);
int GameSave(Game game, char* file);
int GameLoad(Game* data, char* file);
const extern ChessType PlayerChessTypes[2];
#endif