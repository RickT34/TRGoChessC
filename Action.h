#ifndef _ActionH
#define _ActionH
#include "ChessBoard.h"

/**********************落子动作相关******************************/
typedef struct
{
    Point point;       // 落子位置
    int playerid;      // 玩家id
} *Action, Action_Raw; // 落子动作类型

/// @brief 实例化新的落子动作
/// @param playerid  玩家id
/// @param point 落子位置
/// @return 落子动作
Action NewAction(int playerid, Point point);
/// @brief 保存落子动作到字符串
/// @param act 落子动作
/// @param file 要写入的字符串，容量至少为8
/// @return 写入的字符数
int ActionSave(Action act, char *file);
/// @brief 从字符串读取落子动作
/// @param act 读取到的落子动作，不用预先实例化
/// @param file 要读取的字符串
/// @return 读取的字符数
int ActionLoad(Action *act, char *file);
/// @brief 从字符串读取落子动作
/// @param act 读取到的落子动作，不用预先实例化
/// @param file 要读取的字符串
/// @return 读取的字符数
int ActionRawLoad(Action_Raw *act, char *file);

#endif