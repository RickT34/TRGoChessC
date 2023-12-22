#ifndef _GameManagerH
#define _GameManagerH
#include "ACAutomaton.h"
#include "Player.h"
#include "Stack.h"
#include "ChessBoard.h"
#include "Globals.h"
#include "Action.h"
/*******************棋局运行相关**************************/
typedef enum
{
    GameStatus_Begin,                                                                           // 开始
    GameStatus_Gaming,                                                                          // 进行中
    GameStatus_End,                                                                             // 结束
    GameStatus_Pause = (1 << 4)                                                                 // 暂停
} GameStatus;                                                                                   // 棋局状态
#define IsGameStopped(game) (game->status & GameStatus_Pause) || game->status == GameStatus_End // 判断游戏是否中止

typedef struct
{
    ChessBoard chessboard; // 棋盘
    Stack history;         // 落子动作历史
    Player players[2];     // 玩家，id: 0-先手，1-后手
    int nowPlayerID;       // 当前落子玩家id
    GameStatus status;     // 棋局状态
} *Game;                   // 棋局

/// @brief 实例化新棋局
/// @param player1 先手玩家
/// @param player2 后手玩家
/// @return 棋局
Game NewGame(Player player1, Player player2);
/// @brief 释放棋局，玩家不会被释放
/// @param game 棋局
void FreeGame(Game game);
/// @brief 暂停棋局
/// @param game 棋局
/// @return 如果棋局已经终止，返回1；否则返回0
int PauseGame(Game game);
/// @brief 解除暂停棋局
/// @param game 棋局
/// @return 如果棋局没有暂停，返回1；否则返回0
int ContinueGame(Game game);
/// @brief 棋局相关初始化
void GameManagerInit();
/// @brief 进行下一位玩家落子
/// @param game 棋局
/// @return 如果棋局已经终止，返回2；如果落子完成后棋局结束，返回1；否则返回0
int GameNextTurn(Game game);
/// @brief 悔棋(一轮)
/// @param game 棋局
/// @return 如果不能悔棋，返回1；否则返回0
int GameUndo(Game game);
/// @brief 保存棋局到字符串
/// @param game 棋局
/// @param file 要写入字符串，容量应当足够大
/// @return 写入的字符数
int GameSave(Game game, char *file);
/// @brief 从字符串读取棋局
/// @param data 读取的棋局，不用预先实例化
/// @param file 读取的字符串
/// @return 读取的字符数
int GameLoad(Game *data, char *file);

#define GameGetNextPlayer(game) (game->players[GameNextPlayerID(game->nowPlayerID)]) // 获取棋局的下一位玩家
#define GameGetNowPlayer(game) (game->players[game->nowPlayerID])                    // 获取棋局的当前玩家

/*******************棋局复盘相关*************************/
typedef struct
{
    Action_Raw data[BLEN]; // 落子数据
    int datalen;           // 落子数据长度
} *GameRecord;             // 棋局复盘器

/// @brief 从棋局创建棋局复盘器
/// @param game 棋局
/// @return 棋局复盘器
GameRecord NewGameRecord(const Game game);
/// @brief 释放棋局复盘器
/// @param gr 棋局复盘器
void FreeGameRecord(GameRecord gr);
/// @brief 保存棋局复盘器到字符串
/// @param gr 棋局复盘器
/// @param file 写入的字符串，容量应当足够大
/// @return 写入的字符数
int GameRecordSave(const GameRecord gr, char *file);
/// @brief 从字符串读取棋局复盘器
/// @param gr 读取的棋局复盘器，不用预先实例化
/// @param file 读取的字符串
/// @return 读取的字符数
int GameRecordLoad(GameRecord *gr, char *file);
/// @brief 获取第flame步棋后的棋盘
/// @param gr 棋局复盘器
/// @param flame 棋盘序号
/// @return 第flame步棋后的棋盘；如果失败则返回NULL
ChessBoard GameRecordRead(const GameRecord gr, const int flame);
#endif