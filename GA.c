#include "GA.h"
#include <stdio.h>
#include <stdlib.h>
#include<time.h>

#define InstanceTest(p) (rand() <= RAND_MAX * (p))
#define NewArray(an, count) (malloc(sizeof(an) * count))

typedef struct
{
    GAScore score;
    GAGene gene;
} GAIndividual;

int scorecamp(const void* a, const void* b)
{
    GAScore s = ((GAIndividual*)b)->score - ((GAIndividual*)a)->score;
    return s > 0 ? 1 : -1;
}

void CountFitness(const GAConfig config, GAIndividual* inds, const int count)
{
    GAGene* allind = NewArray(GAGene, count);
    for (int i = 0; i < count; ++i) {
        allind[i] = inds[i].gene;
    }
    if (config->GetAllFitness != NULL) {
        GAScore* re = config->GetAllFitness(allind, count);
        for (int i = 0; i < count; ++i) {
            inds[i].score = re[i];
        }
        free(re);
    } else {
        for (int i = 0; i < count; ++i) {
            inds[i].score = config->GetOneFitness(inds[i].gene);
        }
    }
    free(allind);
}

GAIndividual* GANextGen(const GAConfig config, GAIndividual inds[], const int count, const int elitismcount)
{
    qsort(inds, count, sizeof(GAIndividual), scorecamp);
    GAScore scoresum = 0;
    GAScore* cumsum = NewArray(GAScore, count);
    for (int i = 0; i < count; ++i) {
        scoresum += inds[i].score;
        cumsum[i] = scoresum;
    }
    GAIndividual* newinds = NewArray(GAIndividual, count);

    // 1. Clone
    for (int i = 0; i < elitismcount; ++i) {
        newinds[i].gene = config->GetClone(inds[i].gene);
    }
    for (int i = elitismcount; i < count; ++i) {
        GAScore who = (double)rand() * scoresum / RAND_MAX;
        int id = 0;
        while (cumsum[id] < who && id < count - 1)
            ++id;
        newinds[i].gene = config->GetClone(inds[id].gene);
    }
    free(cumsum);
    // 2. Hybrid
    int selected = elitismcount > 0 ? rand() % elitismcount : -1;
    for (int i = elitismcount; i < count; ++i) {
        if (InstanceTest(config->ProbabilityOfHybrid)) {
            if (selected != -1) {
                if (selected >= elitismcount) {
                    GAGene son1 = config->GetHybrid(newinds[selected].gene, newinds[i].gene);
                    config->DeleteGene(newinds[selected].gene);
                    newinds[selected].gene = son1;
                }
                GAGene son2 = config->GetHybrid(newinds[selected].gene, newinds[i].gene);
                config->DeleteGene(newinds[i].gene);
                newinds[i].gene = son2;
                selected = -1;
            } else {
                selected = i;
            }
        }
    }
    // 3. Variation
    for (int i = elitismcount; i < count; ++i) {
        if (InstanceTest(config->ProbabilityOfVariation)) {
            GAGene newi = config->GetVariation(newinds[i].gene);
            config->DeleteGene(newinds[i].gene);
            newinds[i].gene = newi;
        }
    }
    // 4. Count fitness
    CountFitness(config, newinds, count);
    return newinds;
}

GAGene GARun(GAInitData init, int display)
{
    GAIndividual* inds = NewArray(GAIndividual, init->GeneCount);
    GAConfig config = init->Config;
    for (int i = 0; i < init->GeneCount; ++i) {
        inds[i].gene = config->GetClone(init->StartGene[i]);
    }
    CountFitness(init->Config, inds, init->GeneCount);
    int maxi;
    for (int g = 1; g <= init->MAXGenerations; ++g) {
        if (display)
            printf("Generation: %d / %d\n", g,init->MAXGenerations);
        clock_t startt=clock();
        GAIndividual* nxt = GANextGen(config, inds, init->GeneCount, init->ElitismCount);
        startt=clock()-startt;
        if(display)
            printf("Time cost: %.3f sec.\n",(float)startt/CLOCKS_PER_SEC);
        GAScore maxn, minn;
        maxn = minn = nxt[0].score;
        maxi = 0;
        for (int i = 0; i < init->GeneCount; ++i) {
            if (nxt[i].score > maxn) {
                maxn = nxt[i].score;
                maxi = i;
            }
            if (nxt[i].score < minn)
                minn = nxt[i].score;
            config->DeleteGene(inds[i].gene);
        }
        free(inds);
        if (display)
            printf("Max: %lf, Min:%lf\n", maxn, minn);
        inds = nxt;
    }
    GAGene re = inds[maxi].gene;
    for (int i = 0; i < init->GeneCount; ++i) {
        if (i != maxi)
            config->DeleteGene(inds[i].gene);
    }
    free(inds);
    return re;
}

// GAScore fitnesstest(const GAGene ind)
// {
//     return -(ind->x * ind->x + ind->y * ind->y - 2 * ind->x-5*ind->y);
// }

// GAGene GetClonetest(const GAGene ind)
// {
//     GAGene re = malloc(sizeof(*re));
//     re->x = ind->x;
//     re->y = ind->y;
//     return re;
// }
// GAGene GetVariationtest(const GAGene ind)
// {
//     GAGene re = GetClonetest(ind);
//     re->x += ((double)rand() / RAND_MAX - 0.5) * 2;
//     re->y += ((double)rand() / RAND_MAX - 0.5) * 2;
//     return re;
// }
// GAGene GetHybridtest(const GAGene ind1, const GAGene ind2)
// {
//     double p = (double)rand() / RAND_MAX;
//     GAGene re = malloc(sizeof(*re));
//     re->x = ind1->x * p + ind2->x * (1 - p);
//     re->y = ind1->y * p + ind2->y * (1 - p);
//     return re;
// }
// void DeleteGenetest(GAGene ind) { free(ind); }

// void GATest()
// {
//     GAConfig config = malloc(sizeof(*config));
//     config->ProbabilityOfHybrid = 0.25;
//     config->ProbabilityOfVariation = 0.01;
//     config->GetClone = GetClonetest;
//     config->GetOneFitness = fitnesstest;
//     config->GetAllFitness = NULL;
//     config->GetVariation = GetVariationtest;
//     config->GetHybrid = GetHybridtest;
//     config->DeleteGene = DeleteGenetest;
//     GAInitData init = malloc(sizeof(*init));
//     init->MAXGenerations = 1000;
//     init->Config = config;
//     init->ElitismCount = 2;
//     int count = 20;
//     GAGene starts[20];
//     for (int i = 0; i < count; ++i) {
//         GAGene re = malloc(sizeof(*re));
//         re->x = rand() * 10.0 / RAND_MAX;
//         re->y = rand() * 10.0 / RAND_MAX;
//         starts[i] = re;
//     }
//     init->StartGene = starts;
//     init->GeneCount = count;
//     GAGene re = GARun(init, 1);
//     printf("Result: %f,%f\n", re->x, re->y);
//     free(config);
//     free(init);
// }