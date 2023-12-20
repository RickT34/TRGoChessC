#include "AIPlayer.h"
#include "AIUtilities.h"
#include <assert.h>
#include <string.h>

#define Power_MAX (1e30)
#define Power_WIN (1e10)
#define PatternLen (AIPatternLen * 2)

const char *AIPatterns_Default[] = {
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

    "11111"};
const char *AIPatterns_Pruned[] = {
    "110",
    "1110",
    "11010",
    "10110",
    "11110",
    "11111"};
Trie AIPatterns[2];
const Power AIPatternPowers_Naive[] = {
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    1 << 20,

    1 << 20};
const Power AIPatternPowers_Default[] = {
    1,
    1,
    2,
    3,

    15,
    32,
    14,
    28,

    14,
    28,
    32,
    1024,

    Power_WIN};

const Power AIPatternPowers_Default_G1[] = {
    1.264958, 1.128135, 2.360985, 3.413630, 14.196601, 32.206841, 15.003711, 25.843367, 14.462551, 25.108482, 33.186558, 1108.496338, 10000000000.000000};

const Power AIPatternPowersPruned_Default[] = {
    1, 15, 14, 14, 32, 1e10};
const Power AIPatternPowersPruned_Default_G1[] = {
    1.149223, 15.488806, 12.214859, 6.216610, 22.963440, 10753844224.000000};
const Power AIPatternPowersPruned_Default_G2[] = {
    1.168401, 15.162420, 14.000000, 8.567827, 21.539915, 8915616768.000000}; // Good
const Power AIPatternPowersPruned_Default_G3[] = {
    1.215701, 14.281790, 14.783039, 9.156700, 22.695276, 9806633984.000000};

Power UpdatePowerPoint(const Point p, AIData aidata, const ChessBoard cb)
{
    int patn[PatternLen];
    PowerMap pm = aidata->powerMap;
    Power powersum = pm->powerSum;
    for (int d = 0; d < DireLen; ++d)
    {
        memset(patn, 0, sizeof(patn));
        TrieQuery(&GetChess(cb, CBINF[p].start[d]),
                  DireSteps[d], CBINF[p].lens[d], aidata->patterns, patn);
        powersum -= *(pm->linePower[p][d]);
        Power ls = 0;
        for (int k = 0; k < PatternLen; ++k)
        {
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
    for (int d = 0; d < DireLen; ++d)
    {
        memset(patn, 0, sizeof(patn));
        TrieQuery(&GetChess(cb, CBINF[p].start[d]),
                  DireSteps[d], CBINF[p].lens[d], aidata->patterns, patn);
        powersum -= *(pm->linePower[p][d]);
        Power ls = 0;
        for (int k = 0; k < PatternLen; ++k)
        {
            ls += aidata->patternPowers[k] * patn[k];
        }
        powersum += ls;
    }
    return powersum;
}
Point Minimax(const AIData aidata, ChessBoard cb, const char player, const char sgn, Power *rate, const Power maxpower, const char dep)
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        SetChess(cb, p, PlayerChessTypes[player]);
        Power ret, power;
        // PrintChessBoard(cb, ChessBoardStyle_Classic);
        if (dep == 0)
        {
            /*            Compute Power             */
            ret = ComputePowerPoint(p, aidata, cb);
            if (sgn)
                ret = -ret;
        }
        else
        {
            /*             DFS                 */
            // Push
            NeighborMapAddChess(aidata->neighborMap, p);
            PowerMap pm = aidata->powerMap;
            Power linep[DireLen], powersum = pm->powerSum;
            for (int d = 0; d < DireLen; ++d)
                linep[d] = *(pm->linePower[p][d]);
            // End Push
            power = UpdatePowerPoint(p, aidata, cb);
            if (sgn)
                power = -power;
            if (power < Power_WIN)
            {
                if (Minimax(aidata, cb, GameNextPlayerID(player), sgn ^ 1, &ret,
                            re == PointNULL ? Power_MAX : -*rate, dep - 1) != PointNULL)
                {
                    ret = -ret;
                    // if (ret >= Power_WIN)
                    //     ret = Power_WIN;
                }
                else
                {
                    ret = *rate;
                }
            }

            // PrintChessBoard(cb, ChessBoardStyle_Classic);
            // PrintNeighborMap(aidata->neighborMap);

            // Pop
            for (int d = 0; d < DireLen; ++d)
                *(pm->linePower[p][d]) = linep[d];
            pm->powerSum = powersum;
            NeighborMapUndo(aidata->neighborMap, p);
            // End Pop
        }
        if (power >= Power_WIN)
        {
            *rate = power;
            SetChess(cb, p, BLANK);
            if(power>=maxpower){
                return PointNULL;
            }
            re=p;
            return re;
        }
        if (re == PointNULL || ret > *rate)
        {
            *rate = ret;
            if (*rate >= maxpower)
            {
                SetChess(cb, p, BLANK);
                return PointNULL;
            }
            re = p;
        }
        SetChess(cb, p, BLANK);
    }
    // if(maxn!=0)printf("%d %d\n",dep,maxn);
    // printf("Best PointL %d%c",PointTo2C(re));
    return re;
}
void SortPoint(Point *points,const int count,const PowerMap pm)
{
    Power *powers = calloc(count, sizeof(Power));
    for (int i = 0; i < count; ++i)
    {
        for (int d = 0; d < DireLen; ++d)
        {
            powers[i] += *(pm->linePower[points[i]][d]);
        }
        if(powers[i]<0)powers[i]=-powers[i];
    }
    for (int i = 1; i < count; ++i)
    {
        Power key = powers[i];
        Point p=points[i];
        int j = i - 1;
        while (j >= 0 && powers[j] < key)
        {
            powers[j + 1] = powers[j];
            points[j + 1] = points[j];
            j--;
        }
        powers[j + 1] = key;
        points[j + 1] = p;
    }
    free(powers);
}
Point GetBestMove(const AIData aidata, ChessBoard cb, Power *rate, const char dep)
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    int player=aidata->playerid;
    Point points[LLN*LLN];
    int count=0;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        points[count++]=p;
    }
    SortPoint(points,count,aidata->powerMap);
    for (int ip=0;ip<count;++ip)
    {
        Point p=points[ip];
        SetChess(cb, p, PlayerChessTypes[player]);
        Power ret, power;
        // Push
        NeighborMapAddChess(aidata->neighborMap, p);
        PowerMap pm = aidata->powerMap;
        Power linep[DireLen], powersum = pm->powerSum;
        for (int d = 0; d < DireLen; ++d)
            linep[d] = *(pm->linePower[p][d]);
        // End Push
        power = UpdatePowerPoint(p, aidata, cb);
        if (power >= Power_WIN)
        {
            *rate=power;
            SetChess(cb, p, BLANK);
            return p;
        }
        else
        {
            if (Minimax(aidata, cb, GameNextPlayerID(player), 1, &ret,
                        re == PointNULL ? Power_MAX : -*rate, dep - 1) != PointNULL)
            {
                ret = -ret;
            }
            else
            {
                ret = *rate;
            }
        }

        // PrintChessBoard(cb, ChessBoardStyle_Classic);
        // PrintNeighborMap(aidata->neighborMap);

        // Pop
        for (int d = 0; d < DireLen; ++d)
            *(pm->linePower[p][d]) = linep[d];
        pm->powerSum = powersum;
        NeighborMapUndo(aidata->neighborMap, p);
        // End Pop

        if (re == PointNULL || ret > *rate)
        {
            *rate = ret;
            re = p;
        }
        SetChess(cb, p, BLANK);
    }
    // if(maxn!=0)printf("%d %d\n",dep,maxn);
    // printf("Best PointL %d%c",PointTo2C(re));
    return re;
}

Point AIGo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    Point re;
    AIData data = (AIData)player->data;
    ChessBoard cb = CloneChessBoard(ct);
    if (actionHistory->Count == 0)
    {
        re = GetPoint(7, 7);
    }
    else
    {
        Point lastpoint = ((Action)StackTop(actionHistory))->point;
        if (data->neighborMap->needflush)
            NeighborMapFlush(data->neighborMap, actionHistory);
        else
        {
            NeighborMapAddChess(data->neighborMap, lastpoint);
        }
        if (data->powerMap->needflush)
            PowerMapFlush(data, ct, PatternLen);
        else
        {
            UpdatePowerPoint(lastpoint, data, ct);
        }
#ifdef DEBUG
        PrintNeighborMap(data->neighborMap);
        PrintPowerMap(data->powerMap);
#endif
        Power rate;
        // re = Minimax(data, cb, data->playerid, 0, &rate, Power_MAX, AIDepth);
        re=GetBestMove(data,cb,&rate,AIDepth);
#ifdef DEBUG
        printfD("Rate:%f\n", rate);
#endif
    }
    assert(re != PointNULL);
    SetChess(cb, re, PlayerChessTypes[data->playerid]);
    UpdatePowerPoint(re, data, cb);
    FreeChessBoard(cb);
    // PrintChessBoard(cb, ChessBoardStyle_Classic);
    NeighborMapAddChess(data->neighborMap, re);
    return re;
}

void AIUndo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    AIData data = (AIData)player->data;
    data->neighborMap->needflush = 1;
    data->powerMap->needflush = 1;
}

int IsParadistr(const char *s, int l)
{
    int i = 0, j = l - 1;
    while (i < j)
    {
        if (s[i] != s[j])
            return 0;
        i += 1;
        j -= 1;
    }
    return 1;
}

void AIInit()
{
    static char inited = 0;
    if (inited)
        return;
    inited = 1;
    for (int playerid = 0; playerid < 2; ++playerid)
    {
        AIPatterns[playerid] = NewTrie();
        char buff1[20], buff2[20];
        int pati = 0;
        for (int i = 0, id = playerid; i < 2; ++i, id = GameNextPlayerID(id))
        {
            for (int j = 0; j < AIPatternLen; ++j)
            {
                int l = strlen(AIUsePattern[j]);
                for (int k = 0; k < l; ++k)
                {
                    switch (AIUsePattern[j][k])
                    {
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
                    buff2[l - 1 - k] = buff1[k];
                }
                buff1[l] = buff2[l] = 0;
                if (strcmp(buff1, buff2))
                {
                    TrieInsert(AIPatterns[playerid], buff2, l, pati);
                }
                TrieInsert(AIPatterns[playerid], buff1, l, pati);
                ++pati;
            }
        }
        assert(pati == PatternLen);
        TrieCompile(AIPatterns[playerid]);
    }
}

AIData NewAIData(const int playerid, const Power *powers)
{
    AIData re = malloc(sizeof(*re));
    re->powerMap = NewPowerMap();
    re->neighborMap = NewNeighborMap();
    re->playerid = playerid;
    // TODO
    re->patternPowers = malloc(sizeof(Power) * PatternLen);
    re->patterns = AIPatterns[playerid];
    char buff1[20], buff2[20];
    int pati = 0;
    for (int i = 0, id = playerid; i < 2; ++i, id = GameNextPlayerID(id))
    {
        for (int j = 0; j < AIPatternLen; ++j)
        {
            re->patternPowers[pati] = (id == playerid ? powers[j] * AIAttackScale : -powers[j] * AIDefendScale);
            ++pati;
        }
    }
    return re;
}
void FreeAIPlayer(Player player)
{
    AIData data = (AIData)player->data;
    free(data->patternPowers);
    FreePowerMap(data->powerMap);
    FreeNeighborMap(data->neighborMap);
    free(data);
    FreePlayer(player);
}
// patterns: 1 for chess, 2 for op, 0 for blank
Player NewAIPlayer(const char *name, const int playerid, const Power *powers)
{
    Player re = NewPlayer(PlayerType_AI, name, strlen(name));
    SetAIPlayer(re, playerid, powers);
    return re;
}

void SetAIPlayer(Player player, const int playerid, const Power *powers)
{
    player->Go = AIGo;
    player->Undo = AIUndo;
    AIData data = NewAIData(playerid, powers);
    data->powerMap->needflush = 1;
    data->neighborMap->needflush = 1;
    player->data = data;
}
