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

#define DEBUG

#ifdef DEBUG
#define printfD(fmt, ...) printf( "> %s: " fmt "\n",__FUNCTION__, ##__VA_ARGS__ )
#endif

#endif