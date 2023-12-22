#ifndef _PlayerH
#define _PlayerH
#include "Action.h"
#include "Stack.h"
#include "ChessBoard.h"
#include "Globals.h"
/*******************玩家相关*********************/
typedef struct _player *Player;                 // 玩家
#define GameNextPlayerID(player) ((player) ^ 1) // 下一位玩家id
#define GamePrePlayerID(player) ((player) ^ 1)  // 前一位玩家id

enum PlayerType
{
    PlayerType_Human, // 人类玩家
    PlayerType_AI     // AI玩家
};

struct _player
{
    char type;                                                                   // 玩家类型
    char *name;                                                                  // 玩家名字
    Point (*Go)(Player player, const ChessBoard ct, const Stack actionHistory);  // 获取玩家落子位置，输入当前棋盘和落子历史
    void (*Undo)(Player player, const ChessBoard ct, const Stack actionHistory); // 发生悔棋时通知玩家，玩家无需对棋盘进行操作
    void *data;                                                                  // 玩家个人数据
};
/// @brief 保存玩家到字符串
/// @param player 玩家
/// @param file 要写人的字符串
/// @return 写入的字符数
int PlayerSave(Player player, char *file);
/// @brief 从字符串读取玩家
/// @param player 读取的玩家，不用预先实例化
/// @param file 读取的字符串
/// @return 读取的字符数
int PlayerLoad(Player *player, char *file);
/// @brief 实例化新玩家
/// @param type 玩家类型
/// @param name 玩家名字
/// @param namelen 玩家名字长度
/// @return 新玩家
Player NewPlayer(char type, const char *name, int namelen);
/// @brief 释放玩家
/// @param player 玩家
void FreePlayer(Player player);

#endif