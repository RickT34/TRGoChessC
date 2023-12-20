#ifndef _globalsH
#define _globalsH
#include <stdio.h>
#include <stdlib.h>

// #define GetNextPlayer(player) ((player) ^ FULL)
#define Min(x, y) ((x) > (y) ? (y) : (x))
#define Max(x, y) ((x) > (y) ? (x) : (y))
#define Abs(x) ((x) > 0 ? (x) : -(x))
void printArray(int* an, int start, int len);
void Input(char* buff, const int buffsize);

int IntSave(int data, char* file);

int IntLoad(int* data, char* file);

typedef float Power;

// #define DEBUG

#define NewArray(an, count) (malloc(sizeof(an) * count))

#define printHead(s) printf("> %s: " s ": ", __FUNCTION__)
#define printfD(fmt, ...) printf("> %s: " fmt, __FUNCTION__, ##__VA_ARGS__)


#endif