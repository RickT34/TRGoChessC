#ifndef _mainH
#define _mainH
#include "AIPlayer.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include "globals.h"
#include"Train.h"
#include"GA.h"
void Input(char* buff, const int buffsize);
void MakeUI(Game game);
void MakeProcessBar(int p, int len, const int width);
void StartGameRecord(Game game);
Point GetPointInput(const Game game, const char* s);
int InputCommamd(Game game, char* buff);
void Start(Game game);
void PrintPlayer(Game game, int id);

#endif