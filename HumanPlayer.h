#ifndef _HumanPlayerH
#define _HumanPlayerH
#include "Player.h"
Point HumanGo(Player player, const ChessBoard ct, const Point lastAction);
void HumanUndo(Player player, const ChessBoard ct, const Point lastAction);
Player NewHumanPlayer(const char* name);
void SetHumanPlayer(Player player);
#endif