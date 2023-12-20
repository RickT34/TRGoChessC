#include "Train.h"
#include "GA.h"
#include "AIPlayer.h"
#include<math.h>
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include "omp.h"
#define VariationPoint 2
#define VariationRange 0.1f
#define StartVariationRange 0.2f
#define AICount 20
#define GENS 10
#define HYBRID 0.4
#define VARIATION 0.1
#define STARTPATTERN AIPatternPowers_Default


// GAGene = Power*

GAScore *GetAllFitness(const GAGene *allind, const int count)
{
    GAScore *re = calloc(count, sizeof(GAScore));
    omp_set_num_threads(16);
#pragma omp parallel for
    for (int i = 0; i < count; ++i)
    {

        for (int j = 0; j < count; ++j)
        {
            if (i == j)
                continue;
            // putchar('.');
            // printf("%d-%d\n",i,j);
            Player aii = NewAIPlayer("", 0, allind[i]);
            Player aij = NewAIPlayer("", 1, allind[j]);
            Game game = NewGame(aii, aij);
            while (game->status != GameStatus_End)
            {
                GameNextTurn(game);
                // if((game->history->Count)&4)
                //     putchar('.');
            }
            // PrintChessBoard(game->chessboard, ChessBoardStyle_Classic);
            GAScore score=game->history->Count/100.0;
            score*=score;
            score=100.0* exp(-1.5*score)+200.0;
            if (game->nowPlayerID == 0)
            {
#pragma omp critical
                {
                    re[i] += score;
                }
            }
            else
            {
#pragma omp critical
                {
                    re[j] += score * 1.2f;
                }
            }
            putchar('.');
            // getchar();
            FreeGame(game);
            FreeAIPlayer(aij);
            FreeAIPlayer(aii);
        }
    }
    putchar('\n');
    return re;
}

GAGene GetClone(const GAGene ind)
{
    Power *re = malloc(sizeof(Power) * AIPatternLen);
    for (int i = 0; i < AIPatternLen; ++i)
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
        int k = rand() % (AIPatternLen-1);
        re[k] *= 1.0f + (rand() / (float)RAND_MAX - 0.5f) * VariationRange*2;
    } while (rand() % VariationPoint);

    return re;
}
GAGene GetHybrid(const GAGene ind1, const GAGene ind2)
{
    Power *re = malloc(sizeof(Power) * AIPatternLen);
    for (int i = 0; i < AIPatternLen; ++i)
    {
        if (rand() % 2 == 0)
        {
            re[i] = ((Power *)ind1)[i];
        }
        else
        {
            re[i] = ((Power *)ind2)[i];
        }
    }
    return re;
}
void DeleteGene(GAGene ind) { free(ind); }

void PrintGene(GAGene gene)
{
    printf("Gene:  {");
    for (int i = 0; i < AIPatternLen; ++i)
    {
        if (i % 4 == 0)
            putchar('\n');
        printf((i == 0 ? "%f" : ", %f"), ((Power *)gene)[i]);
    }
    printf("}\n");
}



void TrainRun()
{
    printf("Start Train!\n");
    srand(time(NULL));
    GAConfig config = malloc(sizeof(*config));
    config->ProbabilityOfHybrid = HYBRID;
    config->ProbabilityOfVariation = VARIATION;
    config->GetClone = GetClone;
    config->GetOneFitness = NULL;
    config->GetAllFitness = GetAllFitness;
    config->GetVariation = GetVariation;
    config->GetHybrid = GetHybrid;
    config->DeleteGene = DeleteGene;
    GAInitData init = malloc(sizeof(*init));
    init->MAXGenerations = GENS;
    init->Config = config;
    init->ElitismCount = 1;
    int count = AICount;
    GAGene starts[AICount];
    for (int i = 0; i < count; ++i)
    {
        starts[i] = GetClone((Power *)STARTPATTERN);
        if(i==0)continue;
        Power *re = starts[i];
        for (int j = 0; j < AIPatternLen-1; ++j)
        {
            re[j] *= 1.0f + (rand() / (float)RAND_MAX - 0.5f) * StartVariationRange*2;
        }
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