#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"
Point AIGo(Player player, const ChessTable ct, const Point lastAction);
void AIUndo(Player player, const ChessTable ct, const Point lastAction);
Player NewAIPlayer();
void SetAIPlayer(Player player);
#endif