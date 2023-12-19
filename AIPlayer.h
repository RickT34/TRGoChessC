#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"

//Default 13, Pruned 6
#define AIPatternLen 6
#define AIAttackScale 2
#define AIDefendScale 3
#define AIDepth 3
#define AIUsePattern AIPatterns_Pruned

const extern char* AIPatterns_Default[];
const extern Power AIPatternPowers_Default[];
const extern Power AIPatternPowers_Naive[];

const extern char *AIPatterns_Pruned[];
const extern Power AIPatternPowersPruned_Default[];

Player NewAIPlayer(const char* name, const int playerid, const Power* powers);
void SetAIPlayer(Player player, const int playerid, const Power* powers);
void AIInit();
void FreeAIPlayer(Player player);
#endif