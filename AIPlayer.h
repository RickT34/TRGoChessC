#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"
Point AIGo(Player player, const ChessBoard ct, const Point lastAction);
void AIUndo(Player player, const ChessBoard ct, const Point lastAction);
Player NewAIPlayer(const char* name);
void SetAIPlayer(Player player);
#endif