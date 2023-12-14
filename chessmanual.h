#ifndef _chessmanualH
#define _chessmanualH

#include "globals.h"
#include "chessboard.h"

typedef int *ChessManual;
typedef int Key;
typedef struct patternlib *PatternLib;
typedef int Line;
ChessManual ChessManualMake(PatternLib pattern);
PatternLib CompilePatterns(char** patterns,int* powers, int size);
Key LineToKey(Line line);
Line StrToLine(char* s);
void printLine(Line line, int n);
void ChessManualInit();

Key ChessTableXRowToKey(ChessTable ct, int x);
Key ChessTableYRowToKey(ChessTable ct, int y);
Key ChessTableDRowToKey(ChessTable ct, int s, ChessType wall);

#define CheckLib(chessmanual, line) (chessmanual)[LineToKey(line)]

#endif