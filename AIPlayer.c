#include "AIPlayer.h"
#include "AIUtilities.h"
#include <assert.h>
#include <string.h>
#include "omp.h"

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

    "10111",
    "11011",

    "11111"};

const Power AIPatternPowers_Default_G1[] = {
    1.264958, 1.128135, 2.360985, 3.413630, 14.196601, 32.206841, 15.003711, 25.843367, 14.462551, 25.108482, 33.186558, 1108.496338, 30.056494, 30.056494, 10000000000.000000, 3.0}; // Very Good
const Power AIPatternPowers_Default_G3[] = {
    1.265765, 1.139401, 2.573636, 3.415462, 14.763461, 32.287624, 14.929572, 25.641521, 14.462551, 24.830317, 30.056494, 11647.95654, 30.056494, 30.056494, 10000000000.000000, 3.0};
const Power AIPatternPowers_Default_G4[] = {
    1.177428, 1.115898, 2.845249, 3.375630, 14.605908, 33.485008, 17.334507, 24.666882, 14.288996, 24.590176, 31.825541, 11751.788086, 26.172821, 28.198782, 10000000000.000000, 3.719885}; // good
const Power AIPatternPowers_Default_G5[] = {
    1.256042, 1.100238, 3.712060, 3.774202, 14.848924, 32.423199, 17.403543, 24.666882, 14.365765, 25.519703, 31.661978, 9890.199219, 19.824423, 30.210306, 10000000000.000000, 3.719885};
const Power AIPatternPowers_Default_G6[] = {
    1.181662, 1.085176, 2.654626, 3.524394, 15.268023, 33.557255, 17.386007, 24.185219, 16.227776, 23.879221, 32.714535, 12127.288086, 25.691860, 28.511547, 10000000000.000000, 3.607878};

Power UpdatePowerPoint(const Point p, AIData aidata, const ChessBoard cb)
{
    PowerMap pm = aidata->powerMap;
    Power powersum = pm->powerSum;
    for (int d = 0; d < DireLen; ++d)
    {
        Power ls = TrieQuery(&GetChess(cb, CBINF[p].start[d]),
                             DireSteps[d], CBINF[p].lens[d], aidata->patterns);
        powersum -= *(pm->linePower[p][d]);
        *(pm->linePower[p][d]) = ls;
        powersum += ls;
    }
    pm->powerSum = powersum;
    return powersum;
}
Power ComputePowerPoint(const Point p, AIData aidata, const ChessBoard cb)
{
    PowerMap pm = aidata->powerMap;
    Power powersum = pm->powerSum;
    for (int d = 0; d < DireLen; ++d)
    {
        Power ls = TrieQuery(&GetChess(cb, CBINF[p].start[d]),
                             DireSteps[d], CBINF[p].lens[d], aidata->patterns);
        powersum -= *(pm->linePower[p][d]);
        powersum += ls;
    }
    return powersum;
}
Point Minimax(const AIData aidata, ChessBoard cb, const char player,
              Power *rate, const Power maxpower, const char dep)
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        Power ret, power;
        SetChess(cb, p, PlayerChessTypes[player]);
        if (dep == 0)
        {
            /*            Compute Power             */
            ret = ComputePowerPoint(p, aidata, cb);
            if (player != aidata->playerid)
                ret = -ret;
        }
        else
        {
            /*             DFS                 */
            // Push

            PowerMap pm = aidata->powerMap;
            Power linep[DireLen], powersum = pm->powerSum;
            for (int d = 0; d < DireLen; ++d)
                linep[d] = *(pm->linePower[p][d]);
            // End Push
            power = UpdatePowerPoint(p, aidata, cb);
            if (player != aidata->playerid)
                power = -power;
            if (power < Power_WIN)
            {
                NeighborMapAddChess(aidata->neighborMap, p);
                if (Minimax(aidata, cb, GameNextPlayerID(player), &ret,
                            re == PointNULL ? Power_MAX : -*rate, dep - 1) != PointNULL)
                {
                    ret = -ret;
                }
                else
                {
                    ret = *rate;
                }
                NeighborMapUndo(aidata->neighborMap, p);
            }
            // Pop
            for (int d = 0; d < DireLen; ++d)
                *(pm->linePower[p][d]) = linep[d];
            pm->powerSum = powersum;
            // End Pop
        }
        if (power >= Power_WIN)
        {
            *rate = power;
            SetChess(cb, p, BLANK);
            if (power >= maxpower)
            {
                return PointNULL;
            }
            return p;
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
    return re;
}

typedef struct
{
    Point point;
    Power power;
} PointWP;

int PointWPComp(const void *a, const void *b)
{
    return (((PointWP *)b)->power - ((PointWP *)a)->power > 0) ? 1 : -1;
}

PointWP *GetSortPoint(const ChessPot pot, const ChessBoard cb, const PowerMap pm, int *pcount)
{
    PointWP *re = malloc(sizeof(PointWP) * LLN * LLN);
    int count = 0;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        re[count].point = p;
        re[count].power = 0;
        for (int d = 0; d < DireLen; ++d)
        {
            re[count].power += (*(pm->linePower[p][d])) * (*(pm->linePower[p][d]));
        }
        ++count;
    }
    *pcount = count;
    qsort(re, count, sizeof(PointWP), PointWPComp);
    return re;
}
Point GetBestMove(const AIData aidata, ChessBoard cb, const int player,
                  Power *rate, const Power maxpower, const char dep, const uint64 zkey)
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    int count;
    PointWP *pointwps = GetSortPoint(pot, cb, aidata->powerMap, &count);
    for (int ip = 0; ip < count; ++ip)
    {
        Point p = pointwps[ip].point;
        SetChess(cb, p, PlayerChessTypes[player]);
        Power ret, power;
        // PushPowerMap
        PowerMap pm = aidata->powerMap;
        Power linep[DireLen], powersum = pm->powerSum;
        for (int d = 0; d < DireLen; ++d)
            linep[d] = *(pm->linePower[p][d]);
        // End Push
        power = UpdatePowerPoint(p, aidata, cb);
        if (player != aidata->playerid)
            power = -power;
        if (power < Power_WIN)
        {
            Point nret;
            uint64 key = ZobristNextKey(aidata->zobristTable, zkey, p, player);
            HashNode zobval = ZobristTableFind(aidata->zobristTable, key);
            if (zobval != NULL)
            {
                ret = zobval->val.power;
            }
            else
            {
                NeighborMapAddChess(aidata->neighborMap, p);
                if (dep > ODepth)
                {
                    nret = GetBestMove(aidata, cb, GameNextPlayerID(player), &ret,
                                       re == PointNULL ? Power_MAX : -*rate, dep - 1, key);
                }
                else
                {
                    nret = Minimax(aidata, cb, GameNextPlayerID(player), &ret,
                                   re == PointNULL ? Power_MAX : -*rate, dep - 1);
                }
                if (nret != PointNULL)
                {
                    ret = -ret;
                    ZobristTableInsert(aidata->zobristTable, key, ret);
                }
                else
                {
                    ret = *rate;
                }
                NeighborMapUndo(aidata->neighborMap, p);
            }
        }

        // Pop
        for (int d = 0; d < DireLen; ++d)
            *(pm->linePower[p][d]) = linep[d];
        pm->powerSum = powersum;

        // End Pop
        if (power >= Power_WIN)
        {
            *rate = power;
            SetChess(cb, p, BLANK);
            free(pointwps);
            if (power >= maxpower)
            {
                return PointNULL;
            }
            return p;
        }
        if (re == PointNULL || ret > *rate)
        {
            *rate = ret;
            if (*rate >= maxpower)
            {
                SetChess(cb, p, BLANK);
                free(pointwps);
                return PointNULL;
            }
            re = p;
        }
        SetChess(cb, p, BLANK);
    }
    free(pointwps);
    return re;
}

Point GetBestMovePara(const AIData aidata, ChessBoard cb, Power *rate, const char dep)
{
    ChessPot pot = aidata->neighborMap->pot;
    Point re = PointNULL;
    int player = aidata->playerid;
    int count;
    PointWP *pointwps = GetSortPoint(pot, cb, aidata->powerMap, &count);
    *rate = 0;
    omp_set_num_threads(CoreCount);
#pragma omp parallel for schedule(dynamic)
    for (int ip = 0; ip < count; ++ip)
    {
        if (*rate >= Power_WIN)
            continue;
        Point p = pointwps[ip].point;
        ChessBoard cbb = CloneChessBoard(cb);
        SetChess(cbb, p, PlayerChessTypes[player]);
        AIData datab = CloneAIData(aidata);
        datab->zobristTable = NewZobristTable();
        Power powernow = UpdatePowerPoint(p, datab, cbb);
        if (powernow >= Power_WIN)
        {
#pragma omp critical
            {
                *rate = powernow;
                re = p;
            }
        }
        else
        {
            Power retb;
            NeighborMapAddChess(datab->neighborMap, p);
            if (GetBestMove(datab, cbb, GameNextPlayerID(datab->playerid), &retb, re == PointNULL ? Power_MAX : -*rate, dep - 1, datab->zobristTable->start) != PointNULL)
            {
                retb = -retb;
#pragma omp critical
                {
                    if (re == PointNULL || retb > *rate)
                    {

                        *rate = retb;
                        re = p;
                    }
                }
            }
        }

        FreeChessBoard(cbb);
        FreeAIData(datab);
    }
    free(pointwps);
    return re;
}

Point AIGo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    Point re;
    AIData data = (AIData)player->data;
    ChessBoard cb = CloneChessBoard(ct);
    if (actionHistory->Count == 0)
    {
        re = GetPoint(LLN / 2, LLN / 2);
    }
    else
    {
        Point lastpoint = ((Action)StackTop(actionHistory))->point;
        if (data->needflush)
        {

            NeighborMapFlush(data->neighborMap, actionHistory);
            PowerMapFlush(data, ct, PatternLen);
            data->needflush = 0;
        }
        else
        {
            NeighborMapAddChess(data->neighborMap, lastpoint);
            UpdatePowerPoint(lastpoint, data, ct);
        }
#ifdef DEBUG
        // PrintNeighborMap(data->neighborMap);
        // PrintPowerMap(data->powerMap);
#endif
        Power rate;
        ZobristClean(data->zobristTable);
#ifdef USEPARA
        re = GetBestMovePara(data, cb, &rate, AIDepth);
#else
        re = GetBestMove(data, cb, data->playerid, &rate, Power_MAX, AIDepth, data->zobristTable->start);
#endif
#ifdef DEBUG
        printfD("Rate:%f\n", rate);
        printfD("Hash count:%d\n", data->zobristTable->count);
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
    data->needflush = 1;
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

Trie GetPatternTrie(const char *patterns[], const int patternlen, const Power *powers, const int playerid)
{
    Trie re = NewTrie();
    char buff1[20], buff2[20];
    int pati = 0;
    for (int i = 0, id = playerid; i < 2; ++i, id = GameNextPlayerID(id))
    {
        for (int j = 0; j < patternlen; ++j)
        {
            int l = strlen(patterns[j]);
            for (int k = 0; k < l; ++k)
            {
                switch (patterns[j][k])
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
                TrieInsert(re, buff2, l, pati);
            }
            TrieInsert(re, buff1, l, pati);
            ++pati;
        }
    }
    assert(pati == patternlen * 2);
    Power *patternPowers = malloc(sizeof(Power) * pati);
    pati = 0;
    for (int i = 0, id = playerid; i < 2; ++i, id = GameNextPlayerID(id))
    {
        for (int j = 0; j < patternlen; ++j)
        {
            patternPowers[pati] = (id == playerid ? powers[j] * 2 : -powers[j] * powers[patternlen]);
            ++pati;
        }
    }
    TrieCompile(re, patternPowers);
    free(patternPowers);
    return re;
}

AIData NewAIData(const int playerid, const Power *powers)
{
    AIData re = malloc(sizeof(*re));
    re->powerMap = NewPowerMap();
    re->neighborMap = NewNeighborMap();
    re->playerid = playerid;
    re->zobristTable = NewZobristTable();
    re->patterns = GetPatternTrie(AIUsePattern, AIPatternLen, powers, playerid);
    return re;
}
void FreeAIPlayer(Player player)
{
    AIData data = (AIData)player->data;
    FreeTrie(data->patterns);
    FreeAIData(data);
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
    data->needflush = 1;
    player->data = data;
}
