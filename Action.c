#include "Action.h"

Action NewAction(int playerid, Point point)
{
    Action action = malloc(sizeof(*action));
    action->playerid = playerid;
    action->point = point;
    return action;
}

int ActionSave(Action act, char* file)
{
    return IntSave((act->point << 4) | act->playerid, file);
}

int ActionLoad(Action* act, char* file)
{
    int a, d;
    a = IntLoad(&d, file);
    *act = NewAction(d & 0xf, d >> 4);
    return a;
}
int ActionRawLoad(Action_Raw* act, char* file)
{
    int a, d;
    a = IntLoad(&d, file);
    act->playerid = d & 0xf;
    act->point = (d >> 4);
    return a;
}
