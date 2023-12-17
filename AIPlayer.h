#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"

//Default 14, Pruned 6
#define AIPatternLen 6
#define AIAttackScale 2
#define AIDefendScale 3
#define AIDepth 3
#define AIUsePattern AIPatterns_Pruned

const extern char* AIPatterns_Default[];
const extern Power AIPatternPowers_Default[];
const extern Power AIPatternPowers_Naive[];
const extern Power AIPatternPowers_G1[];
const extern Power AIPatternPowers_G2[];
const extern Power AIPatternPowers_G3[];
const extern Power AIPatternPowers_G4[];
const extern Power AIPatternPowers_G5[];
const extern Power AIPatternPowers_G6[];
const extern Power AIPatternPowers_G7[];

const extern char *AIPatterns_Pruned[];
const extern Power AIPatternPowersPruned_Default[];
const extern Power AIPatternPowersPruned_G1[];

Player NewAIPlayer(const char* name, const int playerid, const Power* powers);
void SetAIPlayer(Player player, const int playerid, const Power* powers);
void AIInit();
void FreeAIPlayer(Player player);
#endif