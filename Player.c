#include "Player.h"
#include <string.h>

Player NewPlayer(char type, const char *name, int namelen)
{
    Player re = malloc(sizeof(*re));
    re->type = type;
    re->name = malloc((namelen + 1) * sizeof(char));
    for (int i = 0; i < namelen; ++i)
    {
        re->name[i] = name[i];
    }
    re->name[namelen] = 0;
    re->data = NULL;
    return re;
}

void FreePlayer(Player player)
{
    free(player);
}

int PlayerSave(Player player, char *file)
{
    file[0] = player->type + '0';
    return 1;
}