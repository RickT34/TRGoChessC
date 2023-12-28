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
const Power AIPatternPowers_Default_G3h[] = {
    1.265765, 1.139401, 2.573636, 3.415462, 14.763461, 32.287624, 14.929572, 25.641521, 14.462551, 24.830317, 30.056494, 1164.795654, 10000000000.000000}; // Slightly better than G1
const Power AIPatternPowers_Default_WG3h[] = {
    1.265765, 1.139401, 2.573636, 3.415462, 14.763461, 32.287624, 14.929572, 25.641521, 14.462551, 24.830317, 30.056494, 11647.95654, 30.056494, 30.056494, 10000000000.000000, 3.0};
const Power AIPatternPowers_Default_G4[] = {
    1.177428, 1.115898, 2.845249, 3.375630, 14.605908, 33.485008, 17.334507, 24.666882, 14.288996, 24.590176, 31.825541, 11751.788086, 26.172821, 28.198782, 10000000000.000000, 3.719885};//good
const Power AIPatternPowers_Default_G5[] = {
    1.121167, 1.085445, 2.737436, 3.614628, 14.193470, 32.262943, 17.961742, 24.893641, 15.185129, 23.211491, 31.506727, 12089.468750, 26.185627, 29.352303, 10000000000.000000, 3.754817};


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
            NeighborMapAddChess(aidata->neighborMap, p);
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
                if (Minimax(aidata, cb, GameNextPlayerID(player), &ret,
                            re == PointNULL ? Power_MAX : -*rate, dep - 1) != PointNULL)
                {
                    ret = -ret;
                }
                else
                {
                    ret = *rate;
                }
            }
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
void SortPoint(Point *points, const int count, const PowerMap pm)
{
    Power *powers = calloc(count, sizeof(Power));
    for (int i = 0; i < count; ++i)
    {
        for (int d = 0; d < DireLen; ++d)
        {
            powers[i] += (*(pm->linePower[points[i]][d])) * (*(pm->linePower[points[i]][d]));
        }
    }
    for (int i = 1; i < count; ++i)
    {
        Power key = powers[i];
        Point p = points[i];
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
Point GetBestMove(const AIData aidata, ChessBoard cb, Power *rate, const int player, const char dep, const Power maxpower)
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    Point points[LLN * LLN];
    int count = 0;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        points[count++] = p;
    }
    SortPoint(points, count, aidata->powerMap);
    // ZobristTable zt = aidata->zobristTable;
    // ZobristClean(zt);
    for (int ip = 0; ip < count; ++ip)
    {
        Point p = points[ip];
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
        if (player != aidata->playerid)
            power = -power;
        if (power < Power_WIN)
        {
            Point nret;
            if (dep > 1)
            {
                nret = GetBestMove(aidata, cb, &ret, GameNextPlayerID(player), dep - 1, re == PointNULL ? Power_MAX : -*rate);
            }
            else
            {
                nret = Minimax(aidata, cb, GameNextPlayerID(player), &ret,
                               re == PointNULL ? Power_MAX : -*rate, dep - 1);
            }
            if (nret != PointNULL)
            {
                ret = -ret;
            }
            else
            {
                ret = *rate;
            }
        }

        // Pop
        for (int d = 0; d < DireLen; ++d)
            *(pm->linePower[p][d]) = linep[d];
        pm->powerSum = powersum;
        NeighborMapUndo(aidata->neighborMap, p);
        // End Pop
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

Point GetBestMovePara(const AIData aidata, ChessBoard cb, Power *rate, const char dep)
{
    ChessPot pot = aidata->neighborMap->pot;
    Point re = PointNULL;
    int player = aidata->playerid;
    Point points[LLN * LLN];
    int count = 0;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        points[count++] = p;
    }
    SortPoint(points, count, aidata->powerMap);
    *rate = 0;
    omp_set_num_threads(16);
#pragma omp parallel for
    for (int ip = 0; ip < count; ++ip)
    {
        // PrintAIData(aidata);
        if (*rate >= Power_WIN)
            continue;
        Point p = points[ip];
        ChessBoard cbb = CloneChessBoard(cb);
        SetChess(cbb, p, PlayerChessTypes[player]);
        AIData datab = CloneAIData(aidata);
        Power powernow = UpdatePowerPoint(p, datab, cbb);
        NeighborMapAddChess(datab->neighborMap, p);
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
            if (GetBestMove(datab, cbb, &retb, GameNextPlayerID(datab->playerid), dep - 1, re == PointNULL ? Power_MAX : -*rate) != PointNULL)
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
        FreePowerMap(datab->powerMap);
        FreeNeighborMap(datab->neighborMap);
        free(datab);
    }
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
        PrintNeighborMap(data->neighborMap);
        PrintPowerMap(data->powerMap);
#endif
        Power rate;
#ifdef USEPARA
        re = GetBestMovePara(data, cb, &rate, AIDepth);
#else
        re = GetBestMove(data, cb, &rate, data->playerid, AIDepth, Power_MAX);
#endif
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
    // re->zobristTable = NewZobristTable();
    re->patterns = GetPatternTrie(AIUsePattern, AIPatternLen, powers, playerid);
    return re;
}
void FreeAIPlayer(Player player)
{
    AIData data = (AIData)player->data;
    FreePowerMap(data->powerMap);
    FreeNeighborMap(data->neighborMap);
    FreeTrie(data->patterns);
    // FreeZobristTable(data->zobristTable);
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
    data->needflush = 1;
    player->data = data;
}
