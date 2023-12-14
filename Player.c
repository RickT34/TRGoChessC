#include"Player.h"
#include <string.h>

Player NewPlayer(char type, char* name, size_t namelen){
    Player re=malloc(sizeof(char)*namelen);
    re->type=type;
    re->name=malloc(strlen(name)+1);
    strncpy(re->name, name, namelen);
    re->name[namelen]=0;
    re->data=NULL;
    return re;
}

int PlayerSave(Player player, char* file){
    int l=strlen(player->name);
    int r=0;
    file[0]=player->type+'0';
    r+=1;
    r+=IntSave(l, file+r);
    for(int i=0;i<l;++i){
        file[r+i]=player->name[i];
    }
    return r+l;
}

int PlayerLoad(Player *player, char* file){
    char type=file[0]-'0';
    int r=1,l;
    r+=IntLoad(&l, file+r);
    *player=NewPlayer(type, file+r, l);
    return r+l;
}