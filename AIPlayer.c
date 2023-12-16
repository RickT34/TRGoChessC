#include "AIPlayer.h"
#include "AIUtilities.h"
#include <assert.h>
#include <string.h>

#define Power_MAX (2147483647)
#define Power_WinScale (1<<20)
#define PatternLen (AIPatternLen * 2)

const char* AIPatterns_Default[]={
    "210",
    "010",
    "2110",
    "0110",

    "21110",
    "01110",
    "211010",
    "011010",

    "210110",
    "010110",
    "211110",
    "011110",

    "11111"
};
const Power AIPatternPowers_Default[]={
    1,
    2,
    2,
    4,

    15,
    32,
    14,
    28,

    14,
    28,
    32,
    1<<20,

    1<<20
};

Power UpdatePowerPoint(const Point p, AIData aidata, const ChessBoard cb)
{
    int patn[PatternLen];
    PowerMap pm = aidata->powerMap;
    Power powersum = pm->powerSum;
    for (int d = 0; d < DireLen; ++d) {
        memset(patn, 0, sizeof(patn));
        TrieQuery(&GetChess(cb, CBINF[p].start[d]),
            DireSteps[d], CBINF[p].lens[d], aidata->patterns, patn);
        powersum -= *(pm->linePower[p][d]);
        Power ls = 0;
        for (int k = 0; k < PatternLen; ++k) {
            ls += aidata->patternPowers[k] * patn[k];
        }
        *(pm->linePower[p][d]) = ls;
        powersum += ls;
    }
    pm->powerSum = powersum;
    return powersum;
}
Power ComputePowerPoint(const Point p, AIData aidata, const ChessBoard cb)
{
    int patn[PatternLen];
    PowerMap pm = aidata->powerMap;
    Power powersum = pm->powerSum;
    for (int d = 0; d < DireLen; ++d) {
        memset(patn, 0, sizeof(patn));
        TrieQuery(&GetChess(cb, CBINF[p].start[d]),
            DireSteps[d], CBINF[p].lens[d], aidata->patterns, patn);
        powersum -= *(pm->linePower[p][d]);
        Power ls = 0;
        for (int k = 0; k < PatternLen; ++k) {
            ls += aidata->patternPowers[k] * patn[k];
        }
        powersum += ls;
    }
    return powersum;
}

Point Minimax(const AIData aidata, ChessBoard cb, const int player,const int sgn, Power* rate, const Power maxpower, const int dep)
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    Power maxn;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p]) {
        if (GetChess(cb, p) != BLANK)
            continue;
        SetChess(cb, p, PlayerChessTypes[player]);
        // PrintChessBoard(cb, ChessBoardStyle_Classic);
        if (dep == 0) {
            /*            Compute Power             */
            // assert(sgn==1);
            Power powersum = ComputePowerPoint(p, aidata, cb);
            // PrintPowerMap(aidata->powerMap);
            if (powersum >= maxpower) {
                SetChess(cb, p, BLANK);
                return PointNULL;
            }
            if (maxn < powersum || re == PointNULL) {
                re = p;
                maxn = powersum;
                if(maxn>=Power_WinScale){
                    SetChess(cb, p, BLANK);
                    break;
                }
            }
        } else {
            /*             DFS                 */
            Power ret;
            NeighborMapAddChess(aidata->neighborMap, p);
            // Push
            PowerMap pm = aidata->powerMap;
            Power linep[DireLen], powersum = pm->powerSum;
            for (int d = 0; d < DireLen; ++d)
                linep[d] = *(pm->linePower[p][d]);
            UpdatePowerPoint(p, aidata, cb);
            //End Push

            // PrintChessBoard(cb, ChessBoardStyle_Classic);
            // PrintNeighborMap(aidata->neighborMap);
            Point rep = Minimax(aidata, cb, player ^ 1,-sgn, &ret,
                re == PointNULL ? Power_MAX : maxn*(-sgn), dep - 1);

            //Pop
            for (int d = 0; d < DireLen; ++d)
                *(pm->linePower[p][d]) = linep[d];
            pm->powerSum = powersum;
            NeighborMapUndo(aidata->neighborMap, p);
            //End Pop

            if(rep!=PointNULL){
                ret*=sgn;
                if (ret >= maxpower) {
                    SetChess(cb, p, BLANK);
                    return PointNULL;
                }
                if (re == PointNULL || ret > maxn) {
                    maxn = ret*sgn;
                    re = rep;
                }
            }
        }
        SetChess(cb, p, BLANK);
    }
    *rate = maxn;
    // printf("Best PointL %d%c",PointTo2C(re));
    return re;
}

Point AIGo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    AIData data = (AIData)player->data;
    Point lastpoint = ((Action)StackTop(actionHistory))->point;
    if (data->neighborMap->needflush)
        NeighborMapFlush(data->neighborMap, actionHistory);
    else{
        NeighborMapAddChess(data->neighborMap, lastpoint);
    }
    if (data->powerMap->needflush)
        PowerMapFlush(data,ct,PatternLen);
    else{
        UpdatePowerPoint(lastpoint, data, ct);
    }
    // PrintNeighborMap(data->neighborMap);
    // PrintPowerMap(data->powerMap);
    ChessBoard cb = CloneChessBoard(ct);
    Power rate;
    Point re = Minimax(data, cb, data->playerid,1, &rate, Power_MAX, 4);
    assert(re!=PointNULL);
    SetChess(cb, re, PlayerChessTypes[data->playerid]);
    UpdatePowerPoint(re, data, cb);
    // PrintChessBoard(cb, ChessBoardStyle_Classic);
    FreeChessBoard(cb);
    NeighborMapAddChess(data->neighborMap, re);
#ifdef DEBUG
    printfD("Rate:%d\n",rate);
    #endif
    return re;
}

void AIUndo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    AIData data = (AIData)player->data;
    data->neighborMap->needflush = 1;
    data->powerMap->needflush = 1;
}

int IsParadistr(const char* s, int l){
    int i=0,j=l-1;
    while(i<j){
        if(s[i]!=s[j])return 0;
        i+=1;
        j-=1;
    }
    return 1;
}

AIData NewAIData(const int playerid,  const Power* powers)
{
    AIData re = malloc(sizeof(*re));
    re->powerMap = NewPowerMap();
    re->neighborMap = NewNeighborMap();
    re->patterns = NewTrie();
    re->playerid = playerid;
    // TODO
    re->patternPowers = malloc(sizeof(Power) * PatternLen);
    char buff1[20],buff2[20];
    int pati = 0;
    for (int i = 0, id = playerid; i < 2; ++i, id = GameNextPlayerID(id)) {
        for (int j = 0; j < AIPatternLen; ++j) {
            int l = strlen(AIPatterns_Default[j]);
            for (int k = 0; k < l; ++k) {
                switch (AIPatterns_Default[j][k]) {
                case '1':
                    buff1[k] = PlayerChessTypes[id];
                    break;
                case '2':
                    buff1[k] = PlayerChessTypes[GameNextPlayerID(id)];
                    break;
                case '0':
                    buff1[k] = BLANK;
                    break;
                default:
                    assert(0);
                    break;
                }
                buff2[l-1-k]=buff1[k];
            }
            buff1[l]=buff2[l]=0;
            if(strcmp(buff1, buff2)){
                TrieInsert(re->patterns, buff2, l, pati);
            }
            TrieInsert(re->patterns, buff1, l, pati);
            re->patternPowers[pati] = (id == playerid ? powers[j]*AIAttackScale : -powers[j]*AIDefendScale);
            ++pati;
        }
    }
    assert(pati == PatternLen);
    TrieCompile(re->patterns);
    return re;
}
// patterns: 1 for chess, 2 for op, 0 for blank
Player NewAIPlayer(const char* name, const int playerid, const Power* powers)
{
    Player re = NewPlayer(PlayerType_AI, name, strlen(name));
    SetAIPlayer(re, playerid, powers);
    return re;
}

void SetAIPlayer(Player player, const int playerid, const Power* powers)
{
    player->Go = AIGo;
    player->Undo = AIUndo;
    player->data = NewAIData(playerid, powers);
}
