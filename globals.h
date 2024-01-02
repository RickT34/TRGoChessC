#ifndef _globalsH
#define _globalsH
#include <stdio.h>
#include <stdlib.h>
/*********************全局参数**********************/
#define AIDepth 6 // 搜索层数
#define USEPARA   // 并行计算
#define CoreCount 16
#define GameUseAIPower AIPatternPowers_Default_G10
#define ChessBoardUseStyle ChessBoardStyle_Classic_VS
// #define DEBUG

/*********************全局相关**********************/
#define Min(x, y) ((x) > (y) ? (y) : (x)) // 最小值
#define Max(x, y) ((x) > (y) ? (x) : (y)) // 最大值
#define Abs(x) ((x) > 0 ? (x) : -(x))     // 绝对值
extern const char *PlayerNames[];

/// @brief 打印数组
/// @param an 数组
/// @param start 开始位置
/// @param len 长度
void printArray(int *an, int start, int len);
/// @brief 获取一行命令行输入到缓冲区
/// @param buff 缓冲区
/// @param buffsize 缓冲区大小
void Input(char *buff, const int buffsize);
/// @brief 保存一个int到字符串
/// @param data 数据
/// @param file 要写人的字符串，应当保证容量至少为8
/// @return 写入的字符数
int IntSave(int data, char *file);
/// @brief 从字符串读取一个int
/// @param data 读取到的int
/// @param file 读取的字符串
/// @return 读取的字符数
int IntLoad(int *data, char *file);

typedef float Power; // AI模式匹配权值数据类型
typedef unsigned int uint;
typedef unsigned long long uint64;
typedef unsigned char byte;

#define NewArray(an, count) (malloc(sizeof(an) * count)) // 动态创建数组

#ifdef DEBUG
#define printHead(s) printf("> %s: " s ": ", __FUNCTION__)
#define printfD(fmt, ...) printf("> %s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif

#endif