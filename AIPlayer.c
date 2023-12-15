#include "AIPlayer.h"
#include "AIUtilities.h"
#include <string.h>
#define PatternLen 10
#define Power_MAX (1<<31)
#define Power_WinScale (1<<20)

Power UpdatePowerPoint(const Point p, AIData aidata, const ChessBoard cb)
{
    static int patn[PatternLen];
    PowerMap pm=aidata->powerMap;
    Power powersum = pm->powerSum;
    memset(patn, 0, sizeof(patn));
    for (int d = 0; d < DireLen; ++d) {
        TrieQuery(&GetChess(cb, CBINF[p].start[d]),
            DireSteps[d], CBINF[p].lens[d], aidata->patterns, patn);
        powersum -= *(pm->linePower[p][d]);
    }
    for (int k = 0; k < PatternLen; ++k) {
        powersum += aidata->patternPowers[k] * patn[k];
    }
    pm->powerSum=powersum;
    return powersum;
}

Point Minimax(const AIData aidata,ChessBoard cb,const int player, Power *rate, const Power maxpower,const int sgn,const int dep){
    NeighborMap nbm=aidata->neighborMap;
    ChessPot pot=nbm->pot;
    Point re=PointNULL;
    Power maxn;
    for (Point p = pot->nodes[ChessPotHead].nxt; p != ChessPotTail; p = pot->nodes[p].nxt) {
        if(GetChess(cb, p)!=BLANK)continue;
        SetChess(cb, p, PlayerChessTypes[player]);
        if(dep==1){
            /*            Compute Power             */
            Power powersum=UpdatePowerPoint(p, aidata, cb);
            powersum*=sgn;
            if(powersum>=maxpower){
                SetChess(cb, p, BLANK);
                return PointNULL;
            }
            if(maxn<powersum||re==PointNULL){
                re=p;
                maxn=powersum;
            }
        }
        else{
            /*             DFS                 */
            Power ret;
            NeighborMapAddChess(aidata->neighborMap, p);
            PowerMap pm = aidata->powerMap;
            Power linep[DireLen],powersum=pm->powerSum;
            for(int d=0;d<DireLen;++d)linep[d]=*(pm->linePower[p][d]);
            UpdatePowerPoint(p, aidata, cb);

            Point rep = Minimax(aidata, cb, player^1, &ret, 
            re==PointNULL?Power_MAX:-maxn, -sgn, dep-1);
            
            for (int d = 0; d < DireLen; ++d)
                *(pm->linePower[p][d])=linep[d];
            pm->powerSum=powersum;
            NeighborMapUndo(aidata->neighborMap, p);
            if (rep != PointNULL) {
                maxn = -ret;
                re = rep;
                if(maxn>=Power_WinScale){
                    SetChess(cb, p, BLANK);
                    break;
                }
            }
        }
        SetChess(cb, p, BLANK);
    }
    *rate=maxn;
    return re;
}

Point AIGo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    AIData data = (AIData)player->data;
    if(data->neighborMap->needflush)NeighborMapFlush(data->neighborMap, actionHistory);
    ChessBoard cb=CloneChessBoard(ct);
    Power rate;
    Point re=Minimax(data, cb, ((Action)StackTop(actionHistory))->playerid^1, &rate, Power_MAX, 1, 5);
    return re;
}

void AIUndo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    ((AIData)player->data)->neighborMap->needflush=1;
}

AIData NewAIData()
{
    AIData re = malloc(sizeof(*re));
    re->powerMap = NewPowerMap();
    re->neighborMap = NewNeighborMap();
    re->patterns = NewTrie();
    //TODO

    return re;
}

Player NewAIPlayer(const char* name)
{
    Player re = NewPlayer(PlayerType_AI, name, strlen(name));
    SetAIPlayer(re);
    return re;
}

void SetAIPlayer(Player player)
{
    player->Go = AIGo;
    player->Undo = AIUndo;
    player->data=NewAIData();
}

