#include "AIPlayer.h"
Point AIGo(Player player, const ChessTable ct, const Point lastAction)
{
    return PointNULL;
}

void AIUndo(Player player, const ChessTable ct, const Point lastAction)
{
}

Player NewAIPlayer()
{
    Player re = NewPlayer(PlayerType_AI, "AI", 2);
    SetAIPlayer(re);
    return re;
}

void SetAIPlayer(Player player)
{
    player->Go = AIGo;
    player->Undo = AIUndo;
}