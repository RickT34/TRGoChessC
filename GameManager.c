#include "AIPlayer.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include <assert.h>


#define StartPlayerid PrePlayerID(0)

const ChessType PlayerChessTypes[2] = { PlayerB, PlayerW };
const char winPatterns[2][5] = { { PlayerB, PlayerB, PlayerB, PlayerB, PlayerB },
    { PlayerW, PlayerW, PlayerW, PlayerW, PlayerW } };

Game NewGame(Player player1, Player player2)
{
    Game re = malloc(sizeof(*re));
    re->chesstable = NewChessTable();
    re->history = NewStack(300);
    re->chesstableInf = NewChessTableInf();
    re->nowPlayerID = 0;
    re->players[0] = player1;
    re->players[1] = player2;
    re->winPattern = NewTrie();
    TrieInsert(re->winPattern, &winPatterns[0][0], 5, 0);
    TrieInsert(re->winPattern, &winPatterns[1][0], 5, 1);
    TrieCompile(re->winPattern);
    return re;
}

Player GameGetNextPlayer(Game game)
{
    return game->players[NextPlayerID(game->nowPlayerID)];
}

Player GameGetNowPlayer(Game game){
    return game->players[game->nowPlayerID];
}

void FreeGame(Game game)
{
    FreeChessTable(game->chesstable);
    for (int i = 0; i < game->history->Count; ++i) {
        free(game->history->Items[i]);
    }
    FreeStack(game->history);
    FreeChessTableInf(game->chesstableInf);
    FreeTrie(game->winPattern);
    free(game);
}

Action NewAction(int playerid, Point point)
{
    Action action = malloc(sizeof(*action));
    action->playerid = playerid;
    action->point = point;
    return action;
}

int GameNextTurn(Game game)
{
    if (game->nowPlayerID < 0)
        return 2;
    game->nowPlayerID = NextPlayerID(game->nowPlayerID);
    Point last = game->history->Count ? ((Action)StackTop(game->history))->point : PointNULL;
    Player player = game->players[game->nowPlayerID];
    Point act = player->Go(player, game->chesstable, last);
    assert(GetChess(game->chesstable, act) == BLANK);
    SetChess(game->chesstable, act, PlayerChessTypes[game->nowPlayerID]);
    Action action = NewAction(game->nowPlayerID, act);
    StackPush(game->history, action);
    int win[2];
    for (int d = 0; d < DireLen; ++d) {
        win[0] = win[1] = 0;
        TrieQuery(&GetChess(game->chesstable, game->chesstableInf[act].start[d]),
            DireSteps[d], game->chesstableInf[act].lens[d], game->winPattern, win);
        if (win[0] || win[1]) {
            game->nowPlayerID = ~game->nowPlayerID;
            return 1;
        }
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
    player->Undo(player, game->chesstable, act->point);
    if (game->history->Count == 0)
        game->nowPlayerID = StartPlayerid;
    else
        game->nowPlayerID = ((Action)StackTop(game->history))->playerid;
    SetChess(game->chesstable, act->point, BLANK);
    free(act);
    return 0;
}

int ActionSave(Action act, char* file)
{
    return IntSave((act->point << 4) | act->playerid, file);
}

int ActionLoad(Action* act, char* file)
{
    int a, d;
    a = IntLoad(&d, file);
    *act = NewAction(d & 0xf, d >> 4);
    return a;
}

int GameSave(Game game, char* file)
{
    int i = 0;
    i += ChessTableSave(game->chesstable, file + i);
    i += IntSave(game->history->Count, file + i);
    for (int j = 0; j < game->history->Count; ++j) {
        i += ActionSave((Action)(game->history->Items[j]), file + i);
    }
    i += PlayerSave(game->players[0], file + i);
    i += PlayerSave(game->players[1], file + i);
    i += IntSave(game->nowPlayerID, file + i);
    return i;
}

void FixPlayer(Player player)
{
    if (player->type == 0) {
        SetHumanPlayer(player);
    } else if (player->type == 1) {
        SetAIPlayer(player);
    }
}

int GameLoad(Game* data, char* file)
{
    int i = 0;
    Game re = malloc(sizeof(*re));
    i += ChessTableLoad(&re->chesstable, file + i);
    re->history = NewStack(300);
    int count;
    i += IntLoad(&count, file + i);
    for (int j = 0; j < count; ++j) {
        i += ActionLoad((Action*)&re->history->Items[j], file + i);
    }
    re->chesstableInf = NewChessTableInf();
    re->nowPlayerID = 0;
    Player player1, player2;
    i += PlayerLoad(&player1, file + i);
    i += PlayerLoad(&player2, file + i);
    FixPlayer(player1);
    FixPlayer(player2);
    re->players[0] = player1;
    re->players[1] = player2;
    re->winPattern = NewTrie();
    TrieInsert(re->winPattern, &winPatterns[0][0], 5, 0);
    TrieInsert(re->winPattern, &winPatterns[1][0], 5, 1);
    TrieCompile(re->winPattern);
    *data = re;
    return i;
}