#ifndef _PlayerH
#define _PlayerH
#include "chessboard.h"

typedef struct _player* Player;

enum PlayerType {
    PlayerType_Human,
    PlayerType_AI
};

struct _player {
    char type;
    char* name;
    Point (*Go)(Player player, const ChessTable ct, const Point lastAction);
    void (*Undo)(Player player, const ChessTable ct, const Point lastAction);
    void* data;
};
int PlayerSave(Player player, char* file);
int PlayerLoad(Player* player, char* file);
Player NewPlayer(char type, char* name, size_t namelen);

#endif