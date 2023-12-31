#include "Train.h"
#include "GA.h"
#include "AIPlayer.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"
#include "mt19937.h"
#include <assert.h>
#define VariationPoint 2
#define VariationRange 0.1f
#define AICount 50
#define GENS 50
#define HYBRID 0.4
#define VARIATION 0.1
#define STARTPATTERN_A AIPatternPowers_Default_G7
#define STARTPATTERN_B AIPatternPowers_Default_G5
#define RACECount (AICount * (AICount - 1))

GAScore *GetAllFitness(const GAGene *allind, const int count)
{
    GAScore *re = calloc(count, sizeof(GAScore));
    int tasks[RACECount][2];
    int tc = 0;
    for (int i = 0; i < count; ++i)
    {
        for (int j = 0; j < count; ++j)
        {
            if (i == j)
                continue;
            tasks[tc][0] = i;
            tasks[tc][1] = j;
            ++tc;
        }
    }
    assert(tc == RACECount);
    omp_set_num_threads(CoreCount);
#pragma omp parallel for schedule(dynamic)
    for (tc = 0; tc < RACECount; ++tc)
    {
        int i = tasks[tc][0];
        int j = tasks[tc][1];
        // putchar('.');
        Player aii = NewAIPlayer("", 0, allind[i]);
        Player aij = NewAIPlayer("", 1, allind[j]);
        Game game = NewGame(aii, aij);
        while (game->status != GameStatus_End)
        {
            GameNextTurn(game);
        }
        // PrintChessBoard(game->chessboard, ChessBoardStyle_Classic);
        GAScore score = game->history->Count / 100.0;
        score *= score;
        GAScore score2 =  exp(-1.5 * score);
        score = 100.0 * score2 + 500.0;
        if (game->nowPlayerID == 0)
        {
#pragma omp critical
            {
                re[i] += score;
                re[j] += (1-score2)*40.0;
            }
        }
        else
        {
#pragma omp critical
            {
                re[j] += score * 1.2;
            }
        }
        putchar('.');
        FreeGame(game);
        FreeAIPlayer(aij);
        FreeAIPlayer(aii);
    }
    putchar('\n');
    return re;
}

GAGene GetClone(const GAGene ind)
{
    Power *re = malloc(sizeof(Power) * (AIPatternLen+1));
    for (int i = 0; i <= AIPatternLen; ++i)
    {
        re[i] = ((Power *)ind)[i];
    }
    return re;
}
GAGene GetVariation(const GAGene ind)
{
    Power *re = GetClone(ind);
    do
    {
        int k = genrand64_int63() % (AIPatternLen);
        if(k==AIPatternLen-1)k=AIPatternLen;
        re[k] *= 1.0 + (genrand64_real1() - 0.5) * VariationRange * 2;
    } while (genrand64_int63() % VariationPoint);

    return re;
}
GAGene GetHybrid(const GAGene ind1, const GAGene ind2)
{
    Power *re = malloc(sizeof(Power) * (AIPatternLen+1));
    for (int i = 0; i <= AIPatternLen; ++i)
    {
        double p = genrand64_real1();
        p=1.0/(1+exp(-20.0*(p-0.5)));
        re[i] = ((Power *)ind1)[i] * p + ((Power *)ind2)[i] * (1 - p);
    }
    return re;
}
void DeleteGene(GAGene ind) { free(ind); }

void PrintGene(const GAGene gene)
{
    printf("Gene:  {");
    for (int i = 0; i <= AIPatternLen; ++i)
    {
        printf((i == 0 ? "%f" : ", %f"), ((Power *)gene)[i]);
    }
    printf("}\n");
}

void TrainRun()
{
    printf("Start Train!\n");
    GAConfig config = malloc(sizeof(*config));
    config->ProbabilityOfHybrid = HYBRID;
    config->ProbabilityOfVariation = VARIATION;
    config->GetClone = GetClone;
    config->GetOneFitness = NULL;
    config->GetAllFitness = GetAllFitness;
    config->GetVariation = GetVariation;
    config->GetHybrid = GetHybrid;
    config->DeleteGene = DeleteGene;
    config->PrintGene = PrintGene;
    GAInitData init = malloc(sizeof(*init));
    init->MAXGenerations = GENS;
    init->Config = config;
    init->ElitismCount = 1;
    int count = AICount;
    GAGene starts[AICount];
    init_genrand64(time(NULL));
    starts[0] = GetClone((Power *)STARTPATTERN_A);
    starts[1] = GetClone((Power *)STARTPATTERN_B);
    for (int i = 2; i < count; ++i)
    {
        int k=genrand64_int63()%4;
        if(k>1){
            starts[i]=GetHybrid(starts[0],starts[1]);
        }
        else{
            starts[i]=GetVariation(starts[k]);
        }
        // starts[i] = GetClone((Power *)STARTPATTERN);
        // if(i==0)continue;
        // Power *re = starts[i];
        // for (int j = 0; j < AIPatternLen-1; ++j)
        // {
        //     re[j] *= 1.0f + (rand() / (float)RAND_MAX - 0.5f) * StartVariationRange*2;
        // }
        // printf("Start gene %d:\n",i);
        // PrintGene(starts[i]);
    }
    init->StartGene = starts;
    init->GeneCount = count;
    GAGene re = GARun(init, 1);
    printf("Result: \n");
    PrintGene(re);
    free(config);
    free(init);
}