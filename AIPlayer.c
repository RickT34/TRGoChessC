#include "AIPlayer.h"
#include <string.h>
Point AIGo(Player player, const ChessTable ct, const Point lastAction)
{
    return PointNULL;
}

void AIUndo(Player player, const ChessTable ct, const Point lastAction)
{
}

Player NewAIPlayer(const char* name)
{
    Player re = NewPlayer(PlayerType_AI, name, strlen(name));
    SetAIPlayer(re);
    return re;
}

void SetAIPlayer(Player player)
{
    player->Go = AIGo;
    player->Undo = AIUndo;
}