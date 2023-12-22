#include "HumanPlayer.h"
#include <assert.h>
#include <string.h>

Point HumanGo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    Point re = *(Point *)player->data;
    assert(re != PointNULL);
    *(Point *)player->data = PointNULL;
    return re;
}

void HumanUndo(Player player, const ChessBoard ct, const Stack actionHistory)
{
}

Player NewHumanPlayer(const char *name)
{
    Player re = NewPlayer(PlayerType_Human, name, strlen(name));
    SetHumanPlayer(re);
    return re;
}

void SetHumanPlayer(Player player)
{
    player->data = malloc(sizeof(Point));
    player->Go = HumanGo;
    player->Undo = HumanUndo;
}