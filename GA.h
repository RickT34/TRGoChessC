#ifndef _GAH
#define _GAH

typedef double GAScore;
typedef void * GAGene;

typedef struct
{
    double ProbabilityOfVariation;
    double ProbabilityOfHybrid;
    GAScore (*GetOneFitness)(const GAGene ind);
    GAScore* (*GetAllFitness)(const GAGene* allind, const int count);
    GAGene (*GetClone)(const GAGene ind);
    GAGene (*GetVariation)(const GAGene ind);
    GAGene (*GetHybrid)(const GAGene ind1, const GAGene ind2);
    void (*DeleteGene)(GAGene ind);
    void (*PrintGene)(const GAGene ind);
} *GAConfig;

typedef struct
{
    int MAXGenerations;
    GAConfig Config;
    GAGene *StartGene;
    int GeneCount;
    int ElitismCount;
} *GAInitData;

GAGene GARun(GAInitData init, int display);
void GATest();
#endif