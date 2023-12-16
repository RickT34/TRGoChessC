#include"GA.h"
#include<stdlib.h>
#include<stdio.h>
#define InstanceTest(p) (rand()>=RAND_MAX*(p))
#define NewArray(an, count) (malloc(sizeof(an) * count))

typedef struct
{
    GAScore score;
    GAGene gene;
} GAIndividual;

int scorecamp(const void *a, const void *b)
{
    GAScore s = ((GAIndividual*)b)->score - ((GAIndividual*)a)->score;
    return s>0?1:-1;
}

void CountFitness(const GAConfig config, GAIndividual *inds,const int count)
{
    GAGene *allind = NewArray(GAGene, count);
    for (int i = 0; i < count; ++i)
    {
        allind[i] = inds[i].gene;
    }
    for (int i = 0; i < count; ++i)
    {
        inds[i].score = config->GetFitness(inds[i].gene, allind, count);
    }
    free(allind);
}

GAIndividual* GANextGen(const GAConfig config, GAIndividual inds[],const int count){
    qsort(inds,count,sizeof(GAIndividual),scorecamp);
    GAScore scoresum = 0;
    GAScore *cumsum = NewArray(GAScore, count);
    for(int i=0;i<count;++i){
        scoresum+=inds[i].score;
        cumsum[i]=scoresum;
    }
    GAIndividual *newinds=NewArray(GAIndividual,count);
    newinds[0].gene=config->GetClone(inds[0].gene);
    //1. Clone
    for(int i=1;i<count;++i){
        GAScore who=(double)rand()*scoresum/RAND_MAX;
        int id=0;
        while(cumsum[id]<who&&id<count-1)++id;
        newinds[i].gene = config->GetClone(inds[id].gene);
    }
    free(cumsum);
    //2. Hybrid
    int selected=-1;
    for(int i=1;i<count;++i){
        if(InstanceTest(config->ProbabilityOfHybrid)){
            if(selected!=-1){
                GAGene son1=config->GetHybrid(newinds[selected].gene,newinds[i].gene);
                GAGene son2 = config->GetHybrid(newinds[selected].gene, newinds[i].gene);
                config->DeleteGene(newinds[selected].gene);
                config->DeleteGene(newinds[i].gene);
                newinds[selected].gene=son1;
                newinds[i].gene=son2;
                selected=-1;
            }
            else{
                selected=i;
            }
        }
    }
    //3. Variation
    for(int i=1;i<count;++i){
        if(InstanceTest(config->ProbabilityOfVariation)){
            GAGene newi=config->GetVariation(newinds[i].gene);
            config->DeleteGene(newinds[i].gene);
            newinds[i].gene=newi;
        }
    }
    //4. Count fitness
    CountFitness(config,newinds,count);
    return newinds;
}

GAGene GARun(GAInitData init, int display){
    GAIndividual *inds=NewArray(GAIndividual, init->GeneCount);
    GAConfig config=init->Config;
    for(int i=0;i<init->GeneCount;++i){
        inds[i].gene=config->GetClone(init->StartGene[i]);
    }
    CountFitness(init->Config,inds,init->GeneCount);
    int maxi;
    for(int g=1;g<=init->MAXGenerations;++g){
        if(display)printf("Generation: %d ...\n",g);
        GAIndividual *nxt=GANextGen(config,inds,init->GeneCount);
        GAScore maxn,minn;
        maxn=minn=nxt[0].score;
        maxi=0;
        for(int i=0;i<init->GeneCount;++i){
            if(nxt[i].score>maxn){
                maxn=nxt[i].score;
                maxi=i;
                }
            if(nxt[i].score<minn)minn=nxt[i].score;
            config->DeleteGene(inds[i].gene);
        }
        free(inds);
        if(display)printf("MAX: %f, Min:%f\n",maxn,minn);
        inds=nxt;
    }
    GAGene re=inds[maxi].gene;
    for(int i=0;i<init->GeneCount;++i){
        if(i!=maxi)config->DeleteGene(inds[i].gene);
    }
    free(inds);
    return re;
}

GAScore fitnesstest(const GAGene ind, const GAGene *allind, const int count){
    return -1.3*ind*ind+5.9*ind+50;
}

GAGene GetClonetest(const GAGene ind){
    return ind;
}
GAGene GetVariationtest(const GAGene ind){
    return ind + ((double)rand() / RAND_MAX - 0.5) * 2;
}
GAGene GetHybridtest(const GAGene ind1, const GAGene ind2){
    double p = (double)rand() / RAND_MAX;
    return ind1*p+ind2*(1-p);
}
void DeleteGenetest(GAGene ind){}

void GATest(){
    GAConfig config=malloc(sizeof(*config));
    config->ProbabilityOfHybrid=0.25;
    config->ProbabilityOfVariation=0.01;
    config->GetClone=GetClonetest;
    config->GetFitness=fitnesstest;
    config->GetVariation=GetVariationtest;
    config->GetHybrid=GetHybridtest;
    config->DeleteGene=DeleteGenetest;
    GAInitData init=malloc(sizeof(*init));
    init->MAXGenerations=1000;
    init->Config=config;
    int count=20;
    GAGene starts[20];
    for(int i=0;i<count;++i){
        starts[i]=rand()*10.0/RAND_MAX;
    }
    init->StartGene=starts;
    init->GeneCount=count;
    GAGene re=GARun(init,1);
    printf("Result: %f\n",re);
    free(config);
    free(init);
}