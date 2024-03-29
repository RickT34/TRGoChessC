#include "AIPlayer.h"
#include "AIUtilities.h"
#include <assert.h>
#include <string.h>
#include "omp.h"

#define Power_MAX (1e30) // 最大权值
#define Power_WIN (1e10) // 胜利局面得分下界
#define PatternLen (AIPatternLen * 2)
// 模式集:在每个模式串中， 1 代表己方棋子, 2 代表对方棋子, 0 代表空白。对称模式串和对方模式串会自动补全
const char *AIPatterns_Default[] = {
    "210", "010", "2110", "0110",
    "21110", "01110", "21010", "01010",
    "211010", "210110", "010110", "211110",
    "011110",
    "10111", "11011",
    "11111"};
const char *AIPatterns_Default1[] = {
    "10", "010", "110", "0110",
    "1110", "01110", "1010", "01010",
    "11010", "10110", "010110", "11110",
    "011110",
    "10111", "11011",
    "11111"};
const char *AIPatterns_Default2[] = {
    "10", "000100", "110", "001100",
    "1110", "011100", "1010", "001010",
    "11010", "10110", "010110", "11110",
    "011110",
    "10111", "11011",
    "11111"};
const char *AIPatterns_AT[] = {"011110", "011100", "011010", "11110", "11011", "10111", "001100", "001010", "000100", "11111"};
const Power AIPatternPowers_AT[] = {4320, 720, 720, 720, 720, 720, 120, 120, 20, Power_WIN, 2.1};
// 前面n项与模式串一一对应，最后一项为防守系数
const Power AIPatternPowers_Default_G1[] = {
    1.264958, 1.128135, 2.360985, 3.413630, 14.196601, 32.206841, 15.003711, 14.462551, 25.108482, 33.186558, 1108.496338, 30.056494, 30.056494, 10000000000.000000, 3.0}; // Very Good
const Power AIPatternPowers_Default_G4[] = {
    1.177428, 1.115898, 2.845249, 3.375630, 14.605908, 33.485008, 17.334507, 14.288996, 24.590176, 31.825541, 11751.788086, 26.172821, 28.198782, 10000000000.000000, 3.719885}; // Good
const Power AIPatternPowers_Default_G5[] = {
    1.256042, 1.100238, 3.712060, 3.774202, 14.848924, 32.423199, 17.403543, 14.365765, 25.519703, 31.661978, 9890.199219, 19.824423, 30.210306, 10000000000.000000, 3.719885}; // Good for second hand
const Power AIPatternPowers_Default_G7[] = {
    1.209906, 1.108449, 3.638782, 3.376079, 14.737780, 32.473679, 17.396755, 14.287220, 24.598774, 31.283337, 10865.589844, 26.473148, 28.358171, 10000000000.000000, 3.902418}; // good
const Power AIPatternPowers_Default_G110[] = {
    1.147176, 1.017057, 2.872098, 3.774109,
    14.642408, 39.582565, 2.872098, 3.774109,
    17.334492, 16.255480, 24.932312, 31.667524,
    11747.007813,
    18.771877,
    26.783592,
    10000000000.000000, 2.079848}; // good
const Power AIPatternPowers_Default_G11[] = {
    1.147176, 1.017057, 2.872098, 3.774109,
    14.642408, 39.582565, 0.1, 0.1,
    17.334492, 16.255480, 24.932312, 31.667524,
    11747.007813,
    18.771877,
    26.783592,
    10000000000.000000, 2.079848}; // good
const Power AIPatternPowers_Default_G12[] = {1.075846, 1.017445, 2.680722, 3.773936,
                                             13.389605, 41.094067, 2.872098, 3.774109, 17.334492, 16.255482, 24.932304, 31.676657, 11747.032227, 20.092745, 26.783588, 10000000000.000000, 2.246988}; // good
const Power AIPatternPowers_Default_G11_int[] = {
    115, 102, 287, 377,
    1464, 3958, 287, 377,
    1733, 1626, 2493, 3167,
    1174701,
    1877,
    2678,
    10000000, 2}; // good
const Power AIPatternPowers_Default_G11N[] = {
    1.14717600000000, -1.27729500000000, 1.72492200000000, -1.97008700000000,
    11.7703100000000, 10.2977490000000, 0.707865000000000, 0.324265000000000,
    12.7374720000000, 11.3341950000000, -4.63838600000000, 17.0251160000000,
    11683.6727650000, 2.98229300000000, 21.0393960000000, 10000000000.0000, 2.079848};
const Power AIPatternPowers_Default_G12N[] = {
    1.147176, -1.277295, 1.724922, -1.970098, 14.048094, 10.297749, 0.707865, 0.324265, 12.737472, 11.333668, -4.638386, 17.025118, 11683.989258, 2.982293, 21.039396, 10000000000.000000, 2.008779};
const Power AIPatternPowers_Default_G13N[] = {
    1.147176, -1.277295, 1.724922, -1.970098, 14.048094, 10.297749, 0.707865, 0.324265, 12.737472, 11.333668, -4.638386, 17.025118, 11683.989258, 2.982293, 21.039396, 10000000000.000000, 2.008779};
Power UpdatePowerPoint(const Point p, AIData aidata, const ChessBoard cb) // 更新点p所在的四条直线评分
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
Power ComputePowerPoint(const Point p, AIData aidata, const ChessBoard cb) // 计算点p所在的四条直线评分，不改变powermap
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
              Power *rate, const Power maxpower, const char dep) // 普通Minimax算法，带alpha-beta剪枝、胜利局面剪枝
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
            /*            计算局面得分             */
            PowerMap pm = aidata->powerMap;
            ret = pm->powerSum;
            for (int d = 0; d < DireLen; ++d)
            {
                Power ls = TrieQuery(&GetChess(cb, CBINF[p].start[d]),
                                     DireSteps[d], CBINF[p].lens[d], aidata->patterns);
                ret -= *(pm->linePower[p][d]);
                ret += ls;
            }
            if (player != aidata->playerid)
                ret = -ret;
        }
        else
        {
            // 保存当前局面状态
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
                        ret = *rate; // 被剪枝，不更新结果
                    }
                    NeighborMapUndo(aidata->neighborMap, p);
                            }
            // 恢复局面状态
            for (int d = 0; d < DireLen; ++d)
                *(pm->linePower[p][d]) = linep[d];
            pm->powerSum = powersum;
            // End Pop
        }
        if (power >= Power_WIN) // 如果能胜利就直接返回
        {
            SetChess(cb, p, BLANK);
            if (power >= maxpower)
            {
                return PointNULL;
            }
            *rate = power * dep;
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

void AItest(AIData data)
{
    char buff1[20];
    int pati = 0, id = data->playerid;
    for (int j = 0; j < AIPatternLen; ++j)
    {
        int l = strlen(AIPatterns_Default[j]);
        for (int k = 0; k < l; ++k)
        {
            switch (AIPatterns_Default[j][k])
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
        }
        printf("%s : %f\n", AIPatterns_Default[j], TrieQuery(buff1, 1, l, data->patterns));
    }
}

typedef struct
{
    Point point;
    Power power;
} PointWP; // 用于点排序

int PointWPComp(const void *a, const void *b)
{
    return (((PointWP *)b)->power - ((PointWP *)a)->power > 0) ? 1 : -1;
}

void GetSortPoint(const ChessPot pot, const ChessBoard cb, const PowerMap pm, int *pcount, PointWP *re) // 按照点上4条直线的评分平方和从大到小排序点
{
    int count = 0;
    for (Point p = pot->nxtnode[ChessPotHead]; p != ChessPotTail; p = pot->nxtnode[p])
    {
        if (cb[p] != BLANK)
            continue;
        re[count].point = p;
        re[count].power = 0;
        for (int d = 0; d < DireLen; ++d)
        {
            re[count].power += Abs(*(pm->linePower[p][d]));
            // re[count].power +=  (*(pm->linePower[p][d])) * (*(pm->linePower[p][d]));
        }
        ++count;
    }
    qsort(re, count, sizeof(PointWP), PointWPComp);
    *pcount = count;
}
Point GetBestMove(const AIData aidata, ChessBoard cb, const int player,
                  Power *rate, const Power maxpower, const char dep, const uint64 zkey) // 优化Minimax算法，使用alpha-beta剪枝、胜利局面剪枝、启发式搜索、Zobrist哈希
{
    NeighborMap nbm = aidata->neighborMap;
    ChessPot pot = nbm->pot;
    Point re = PointNULL;
    int count;
    PointWP pointwps[LLN * LLN];
    GetSortPoint(pot, cb, aidata->powerMap, &count, pointwps);
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
            SetChess(cb, p, BLANK);
            if (power >= maxpower)
            {
                return PointNULL;
            }
            *rate = power * dep;
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

Point GetBestMovePara(const AIData aidata, ChessBoard cb, Power *rate, const char dep) // 并行的、优化的Minimax算法
{
    ChessPot pot = aidata->neighborMap->pot;
    Point re = PointNULL;
    int player = aidata->playerid;
    int count;
    PointWP pointwps[LLN * LLN];
    GetSortPoint(pot, cb, aidata->powerMap, &count, pointwps);
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
        Power powernow = UpdatePowerPoint(p, datab, cbb);
        if (powernow >= Power_WIN)
        {
#pragma omp critical
            {
                *rate = powernow * 1e10;
                re = p;
            }
        }
        else
        {
            Power retb, pset;
#pragma omp critical
            {
                pset = re == PointNULL ? Power_WIN : -*rate;
            }
            NeighborMapAddChess(datab->neighborMap, p);
            if (GetBestMove(datab, cbb, GameNextPlayerID(datab->playerid), &retb, pset, dep - 1, datab->zobristTable->start) != PointNULL)
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
    if(re==PointNULL)re=pointwps[0].point;
    return re;
}

Point AIGo(Player player, const ChessBoard ct, const Stack actionHistory) // AI落子程序入口
{
    Point re;
    AIData data = (AIData)player->data;
    // AItest(data);
    ChessBoard cb = CloneChessBoard(ct);
    if (actionHistory->Count == 0)
    {
        re = GetPoint(LLN / 2, LLN / 2);
    }
    else
    {
        Point lastpoint = ((Action)StackTop(actionHistory))->point;
        if (data->needflush) // 有悔棋、读取游戏情况是需要重建
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
        ZobristClean(data->zobristTable);
#ifdef USEPARA
        re = GetBestMovePara(data, cb, &rate, AIDepth);
#else
        re = GetBestMove(data, cb, data->playerid, &rate, Power_MAX, AIDepth, data->zobristTable->start);
#endif
        // printf("Rate:%f\n", rate);
#ifdef DEBUG
        printfD("Rate:%f\n", rate);
        printfD("Hash count:%d\n", data->zobristTable->count);
#endif
    }
    assert(re != PointNULL);
    SetChess(cb, re, PlayerChessTypes[data->playerid]);
    UpdatePowerPoint(re, data, cb);
    FreeChessBoard(cb);
    NeighborMapAddChess(data->neighborMap, re);
    return re;
}

void AIUndo(Player player, const ChessBoard ct, const Stack actionHistory)
{
    AIData data = (AIData)player->data;
    data->needflush = 1;
}

int IsParadistr(const char *s, int l) // 判断是否是回文串
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

Trie GetPatternTrie(const char *patterns[], const int patternlen, const Power *powers, const int playerid) // 从模式和权值构建AC自动机
{
    Trie re = NewTrie();
    char buff1[10], buff2[10];
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
            for (int i = 0; i < l; ++i)
            {
                if (buff1[i] != buff2[i])
                {
                    TrieInsert(re, buff2, l, pati);
                    break;
                }
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
