#ifndef _ActionH
#define _ActionH
#include "chessboard.h"

typedef struct {
    Point point;
    int playerid;
} * Action, Action_Raw;

Action NewAction(int playerid, Point point);
int ActionSave(Action act, char* file);
int ActionLoad(Action* act, char* file);
int ActionRawLoad(Action_Raw* act, char* file);

#endif