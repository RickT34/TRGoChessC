#ifndef _GAH
#define _GAH

typedef float GAScore;
typedef float GAGene;

typedef struct
{
    double ProbabilityOfVariation;
    double ProbabilityOfHybrid;
    GAScore (*GetFitness)(const GAGene ind, const GAGene *allind, const int count);
    GAGene (*GetClone)(const GAGene ind);
    GAGene (*GetVariation)(const GAGene ind);
    GAGene (*GetHybrid)(const GAGene ind1, const GAGene ind2);
    void (*DeleteGene)(GAGene ind);
} *GAConfig;

typedef struct
{
    int MAXGenerations;
    GAConfig Config;
    GAGene *StartGene;
    int GeneCount;
} *GAInitData;

GAGene GARun(GAInitData init, int display);
void GATest();
#endif