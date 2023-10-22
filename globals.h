#ifndef _globalsH
#define _globalsH
#include <stdlib.h>
#include <stdio.h>

#define LLN 15
#define FULL 3
#define PlayerW 1//01 O
#define PlayerB 2//10 #
#define BLANK 0
#define BIT 2

typedef int Player;

#define GetNextPlayer(player) ((player)^FULL)
#define Min(x, y) ((x)>(y)?(y):(x))
#define Max(x, y) ((x)>(y)?(x):(y))
#define Abs(x) ((x)>0?(x):-(x) )
#define DEBUG

#ifdef DEBUG
#define printfD(fmt, ...) printf( "> %s: " fmt "\n",__FUNCTION__, ##__VA_ARGS__ )
#define printBits(num) printfD("%32b",num)
#endif

#endif