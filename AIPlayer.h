#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"

#define AIPatternLen (sizeof(AIPatternPowers_Default)/sizeof(Power))
#define AIAttackScale 1
#define AIDefendScale 1
#define AIDepth 5

const extern char* AIPatterns_Default[];
const extern Power AIPatternPowers_Default[];

Player NewAIPlayer(const char* name, const int playerid, const Power* powers);
void SetAIPlayer(Player player, const int playerid, const Power* powers);
#endif