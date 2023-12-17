#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"

#define AIPatternLen 13
#define AIAttackScale 1
#define AIDefendScale 2
#define AIDepth 2

const extern char* AIPatterns_Default[];
const extern Power AIPatternPowers_Default[];
const extern Power AIPatternPowers_Naive[];

Player NewAIPlayer(const char* name, const int playerid, const Power* powers);
void SetAIPlayer(Player player, const int playerid, const Power* powers);
void AIInit();
void FreeAIPlayer(Player player);
#endif