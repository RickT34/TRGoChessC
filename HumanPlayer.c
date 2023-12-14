#include "HumanPlayer.h"
#include <assert.h>

Point HumanGo(Player player, const ChessTable ct, const Point lastAction)
{
    Point re=*(Point*)player->data;
    assert(re!=PointNULL);
    *(Point*)player->data=PointNULL;
    return re;
}

void HumanUndo(Player player, const ChessTable ct, const Point lastAction)
{
}

Player NewHumanPlayer()
{
    Player re = NewPlayer(PlayerType_Human, "Human", 5);
    SetHumanPlayer(re);
    re->data=malloc(sizeof(Point));
    return re;
}

void SetHumanPlayer(Player player){
    player->Go = HumanGo;
    player->Undo = HumanUndo;
}