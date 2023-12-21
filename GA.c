#include "GA.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mt19937.h"

#define InstanceTest(p) (genrand64_real1() <=  (p))
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
        GAScore who = genrand64_real1() * scoresum;
        int id = 0;
        while (cumsum[id] < who && id < count - 1)
            ++id;
        newinds[i].gene = config->GetClone(inds[id].gene);
    }
    free(cumsum);
    // 2. Hybrid
    int selected = elitismcount > 0 ? genrand64_int63() % elitismcount : -1;
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
        }
        if (display)
        {
            printf("Max: %lf, Min:%lf\n", maxn, minn);
            config->PrintGene(nxt[maxi].gene);
        }
        for(int i=0;i<init->GeneCount;++i){
            config->DeleteGene(inds[i].gene);
        }
        free(inds);
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