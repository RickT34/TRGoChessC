

#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"

/*************************搜索参数*****************************/

#define AIPatternLen 16                 // 使用的匹配模式个数
#define AIUsePattern AIPatterns_Default // 使用的匹配模式集
#define ODepth 1

/********************可用的匹配模式及权值************************/
// 标准匹配模式集，个数为 16
const extern char *AIPatterns_Default[];
// const extern char *AIPatterns_Default1[];
// const extern char *AIPatterns_AT[];//10

// 标准匹配模式集，各代权值
// const extern Power AIPatternPowers_Default_G1[];
// const extern Power AIPatternPowers_Default_G3[];

// const extern Power AIPatternPowers_Default_G4[];
// const extern Power AIPatternPowers_Default_G5[];
// const extern Power AIPatternPowers_Default_G6[];
// const extern Power AIPatternPowers_Default_G7[];
// const extern Power AIPatternPowers_Default_G8[];
// const extern Power AIPatternPowers_Default_G9[];
// const extern Power AIPatternPowers_Default_G10[];
const extern Power AIPatternPowers_Default_G11[];
const extern Power AIPatternPowers_Default_G12[];
// const extern Power AIPatternPowers_Default_G11_int[];
// const extern Power AIPatternPowers_Default_G11N[];
// const extern Power AIPatternPowers_Default_G12[];
// const extern Power AIPatternPowers_Default_G13[];


const extern Power AIPatternPowers_AT[];

/***********************AI玩家相关******************************/

/// @brief 实例化新的AI玩家，继承自Player
/// @param name 名字
/// @param playerid 在对局中的玩家id
/// @param powers 使用权值
/// @return
Player NewAIPlayer(const char *name, const int playerid, const Power *powers);
/// @brief 释放AI玩家
/// @param player
void FreeAIPlayer(Player player);
/// @brief 重新设置AI玩家
/// @param player AI玩家
/// @param playerid 在对局中的玩家id
/// @param powers 使用权值
void SetAIPlayer(Player player, const int playerid, const Power *powers);

#endif