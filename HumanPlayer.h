#ifndef _HumanPlayerH
#define _HumanPlayerH
#include "Player.h"
/******************人类玩家相关*******************/

/// @brief 实例化新的人类玩家，继承自Player
/// @param name 玩家名字
/// @return 新的人类玩家
Player NewHumanPlayer(const char *name);
/// @brief 释放人类玩家
/// @param player 人类玩家
void FreeHumanPlayer(Player player);
/// @brief 重置人类玩家
/// @param player 人类玩家
void SetHumanPlayer(Player player);
#endif