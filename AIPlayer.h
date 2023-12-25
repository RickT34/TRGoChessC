

#ifndef _AIPlayerH
#define _AIPlayerH
#include "Player.h"

/*************************搜索参数*****************************/

#define AIPatternLen 13                 // 使用的匹配模式个数
#define AIUsePattern AIPatterns_Default // 使用的匹配模式集
#define AIAttackScale 2                 // 攻击性
#define AIDefendScale 3                 // 防守性
#define AIDepth 4                       // 搜索层数

/********************可用的匹配模式及权值************************/
// 标准匹配模式集，个数为 12
const extern char *AIPatterns_Default[];

// 标准匹配模式集，各代权值
const extern Power AIPatternPowers_Default[];
const extern Power AIPatternPowers_Default_G1[];
const extern Power AIPatternPowers_Default_G2[];
const extern Power AIPatternPowers_Default_G3h[];

// 轻量匹配模式集， 个数为 6
const extern char *AIPatterns_Pruned[];

// 轻量匹配模式集，各代权值
const extern Power AIPatternPowersPruned_Default[];
const extern Power AIPatternPowersPruned_Default_G1[];
const extern Power AIPatternPowersPruned_Default_G2[];

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

/// @brief AI相关初始化
void AIInit();

#endif