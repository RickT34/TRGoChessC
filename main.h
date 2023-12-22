#ifndef _mainH
#define _mainH
#include "AIPlayer.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include "Globals.h"
#include "Train.h"
#include "GA.h"
/********************主程序相关**********************/

/// @brief 创建界面
/// @param game 棋局
void MakeUI(Game game);
/// @brief 创建进度条
/// @param p 进度
/// @param len 总数
/// @param width 进度条宽度
void MakeProcessBar(int p, int len, const int width);
/// @brief 开始复盘
/// @param game 棋局
void StartGameRecord(Game game);
/// @brief 获取合法点输入
/// @param game 棋局
/// @param s 缓冲区
/// @return 点输入；如果点非法，则返回PointNULL
Point GetPointInput(const Game game, const char *s);
/// @brief 获取命令输入
/// @param game 棋局
/// @param buff 缓冲区
/// @return 返回0，如果有命令输入；否则返回1
int InputCommamd(Game game, char *buff);
/// @brief 开始棋局
/// @param game 棋局
void Start(Game game);
/// @brief 打印玩家标识
/// @param game 棋局
/// @param id 玩家id
void PrintPlayer(Game game, int id);

#endif