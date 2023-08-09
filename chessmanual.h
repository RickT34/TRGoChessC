#ifndef _chessmanualH
#define _chessmanualH

#include "globals.h"
#include "chessboard.h"

typedef int *ChessManual;
typedef int Key;
typedef struct patternlib *PatternLib;
typedef int Line;
ChessManual MakeLib(PatternLib pattern);
PatternLib CompilePatterns(char** patterns,int* powers, int size);
Key LineToKey(Line line);
Line StrToLine(char* s);
#define CheckLib(chessmanual, line) (chessmanual)[LineToKey(line)]

#endif