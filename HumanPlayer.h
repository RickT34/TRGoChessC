#ifndef _HumanPlayerH
#define _HumanPlayerH
#include "Player.h"
Point HumanGo(Player player, const ChessTable ct, const Point lastAction);
void HumanUndo(Player player, const ChessTable ct, const Point lastAction);
Player NewHumanPlayer();
void SetHumanPlayer(Player player);
#endif