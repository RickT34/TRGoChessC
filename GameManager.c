#include "AIPlayer.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include <assert.h>

#define StartPlayerid GamePrePlayerID(0)

const char winPatterns[2][5] = {{PlayerB, PlayerB, PlayerB, PlayerB, PlayerB},
                                {PlayerW, PlayerW, PlayerW, PlayerW, PlayerW}};
const Power winpowers[]={1,1};
Trie WinPattern;

void GameManagerInit()
{
    static char inited = 0;
    if (inited)
        return;
    WinPattern = NewTrie();
    TrieInsert(WinPattern, &winPatterns[0][0], 5, 0);
    TrieInsert(WinPattern, &winPatterns[1][0], 5, 1);
    TrieCompile(WinPattern,winpowers);
    inited = 1;
}

Game NewGame(Player player1, Player player2)
{
    Game re = malloc(sizeof(*re));
    re->chessboard = NewChessBoard();
    re->history = NewStack(BLEN);
    re->nowPlayerID = StartPlayerid;
    re->players[0] = player1;
    re->players[1] = player2;
    re->status = GameStatus_Begin;
    return re;
}

void FreeGame(Game game)
{
    FreeChessBoard(game->chessboard);
    for (int i = 0; i < game->history->Count; ++i)
    {
        free(game->history->Items[i]);
    }
    FreeStack(game->history);
    free(game);
}

int GameNextTurn(Game game)
{
    if (IsGameStopped(game))
        return 2;
    game->status = GameStatus_Gaming;
    game->nowPlayerID = GameNextPlayerID(game->nowPlayerID);
    Player player = GameGetNowPlayer(game);
    Point act = player->Go(player, game->chessboard, game->history);

    assert(GetChess(game->chessboard, act) == BLANK);
    SetChess(game->chessboard, act, PlayerChessTypes[game->nowPlayerID]);
    Action action = NewAction(game->nowPlayerID, act);
    StackPush(game->history, action);

    for (int d = 0; d < DireLen; ++d)
    {
        Power win = TrieQuery(&GetChess(game->chessboard, CBINF[act].start[d]),
                  DireSteps[d], CBINF[act].lens[d], WinPattern);
        if (win)
        {
            game->status = GameStatus_End;
            return 1;
        }
    }

    if (game->history->Count == LLN * LLN)
    {
        game->nowPlayerID = 1;
        game->status = GameStatus_End;
        return 1;
    }
    return 0;
}

int GameUndo(Game game)
{
    if (game->history->Count == 0)
        return 1;
    Action act = StackPop(game->history);
    assert(act->playerid == game->nowPlayerID || game->nowPlayerID < 0);
    Player player = game->players[act->playerid];
    player->Undo(player, game->chessboard, game->history);
    if (game->history->Count == 0)
    {
        game->nowPlayerID = StartPlayerid;
        game->status = GameStatus_Begin;
    }
    else
    {
        game->nowPlayerID = ((Action)StackTop(game->history))->playerid;
        game->status = GameStatus_Gaming;
    }
    SetChess(game->chessboard, act->point, BLANK);
    free(act);
    return 0;
}

int PauseGame(Game game)
{
    if (game->status & GameStatus_Pause)
        return 1;
    game->status |= GameStatus_Pause;
    return 0;
}

int ContinueGame(Game game)
{
    if (game->status & GameStatus_Pause)
    {
        game->status ^= GameStatus_Pause;
        return 0;
    }
    return 1;
}

int GameSave(Game game, char *file)
{
    int i = 0;
    i += ChessBoardSave(game->chessboard, file + i);
    i += IntSave(game->history->Count, file + i);
    for (int j = 0; j < game->history->Count; ++j)
    {
        i += ActionSave((Action)(game->history->Items[j]), file + i);
    }
    i += PlayerSave(game->players[0], file + i);
    i += PlayerSave(game->players[1], file + i);
    i += IntSave(game->nowPlayerID, file + i);
    i += IntSave(game->status, file + i);
    return i;
}

void FixPlayer(Player player, int playerid)
{
    if (player->type == PlayerType_Human)
    {
        SetHumanPlayer(player);
    }
    else if (player->type == PlayerType_AI)
    {
        SetAIPlayer(player, playerid, GameUseAIPower);
    }
}

int GameLoad(Game *data, char *file)
{
    int i = 0;
    Game re = malloc(sizeof(*re));
    i += ChessBoardLoad(&re->chessboard, file + i);
    re->history = NewStack(300);
    int count;
    i += IntLoad(&count, file + i);
    for (int j = 0; j < count; ++j)
    {
        i += ActionLoad((Action *)&re->history->Items[j], file + i);
    }
    re->history->Count = count;
    re->nowPlayerID = 0;
    Player player1, player2;
    i += PlayerLoad(&player1, file + i);
    i += PlayerLoad(&player2, file + i);
    FixPlayer(player1, 0);
    FixPlayer(player2, 1);
    re->players[0] = player1;
    re->players[1] = player2;
    i += IntLoad(&re->nowPlayerID, file + i);
    i += IntLoad((int *)&re->status, file + i);
    *data = re;
    return i;
}

GameRecord NewGameRecord(const Game game)
{
    GameRecord re = malloc(sizeof(*re));
    for (int i = 0; i < game->history->Count; ++i)
    {
        re->data[i] = *(Action)game->history->Items[i];
    }
    re->datalen = game->history->Count;
    return re;
}

void FreeGameRecord(GameRecord gr)
{
    free(gr);
}

int GameRecordSave(const GameRecord gr, char *file)
{
    int i = 0;
    i += IntSave(gr->datalen, file + i);
    for (int j = 0; j < gr->datalen; ++j)
    {
        i += ActionSave(&gr->data[j], file + i);
    }
    return i;
}

int GameRecordLoad(GameRecord *gr, char *file)
{
    *gr = malloc(sizeof(**gr));
    int i = 0;
    i += IntLoad(&(*gr)->datalen, file + i);
    for (int j = 0; j < (*gr)->datalen; ++j)
    {
        i += ActionRawLoad(&(*gr)->data[j], file + i);
    }
    return i;
}

ChessBoard GameRecordRead(const GameRecord gr, const int flame)
{
    if (flame >= gr->datalen || flame < -1)
        return NULL;
    ChessBoard re = NewChessBoard();
    for (int i = 0; i <= flame; ++i)
    {
        SetChess(re, gr->data[i].point, PlayerChessTypes[gr->data[i].playerid]);
    }
    return re;
}