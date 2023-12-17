#include"Train.h"
#include "GA.h"
#include "AIPlayer.h"

int TrainGetResult(Game game){
    printfD("Get result...\n");
    while(game->status!=GameStatus_End){
        GameNextTurn(game);
        // PrintChessBoard(game->chessboard,ChessBoardStyle_Classic);
    }
    return game->nowPlayerID;
}

int TrainTestAI(const Power* power1, const Power* power2){
    Player ai0=NewAIPlayer("ai0",0,power1);
    Player ai1=NewAIPlayer("ai1",1,power1);
    Game game=NewGame(ai0,ai1);
    int re=TrainGetResult(game);
    FreeGame(game);
    return re;
}

//GAGene = Power*

GAScore* GetAllFitness(const GAGene* allind, const int count){
    GAScore *re=calloc(count, sizeof(GAScore));
    for(int i=0;i<count;++i){

        for(int j=0;j<count;++j){
            if(i==j)continue;
            printf("%d-%d\n",i,j);
            Player aii = NewAIPlayer("", 0, allind[i]);
            Player aij = NewAIPlayer("", 1, allind[j]);
            Game game=NewGame(aii, aij);
            while (game->status != GameStatus_End) {
                GameNextTurn(game);
                PrintChessBoard(game->chessboard, ChessBoardStyle_Classic);
            }
            if(game->nowPlayerID==0){
                re[i]+=1024.0f/game->history->Count;
            }
            else{
                re[j]+=2048.0f/game->history->Count;
            }
            FreeGame(game);
            FreeAIPlayer(aij);
            FreeAIPlayer(aii);
        }
    }
    return re;
}

GAGene GetClone(const GAGene ind)
{
    Power* re = malloc(sizeof(Power)*AIPatternLen);
    for(int i=0;i<AIPatternLen;++i){
        re[i]=((Power*)ind)[i];
    }
    return re;
}
GAGene GetVariation(const GAGene ind)
{
    Power* re = GetClone(ind);
    do{
        int k=rand()%AIPatternLen;
        re[k] *= 1.0f + (rand() / (float)RAND_MAX -0.5f)*0.2f;
    }while(rand()%2);

    return re;
}
GAGene GetHybrid(const GAGene ind1, const GAGene ind2)
{
    Power* re = malloc(sizeof(Power) * AIPatternLen);
    for(int i=0;i<AIPatternLen;++i){
        if(rand()%2==0){
            re[i]=((Power*)ind1)[i];
        }
        else{
            re[i] = ((Power*)ind2)[i];
        }
    }
    return re;
}
void DeleteGene(GAGene ind) { free(ind); }

void PrintGene(GAGene gene){
    printf("Gene:  {");
    for (int i = 0; i < AIPatternLen; ++i) {
        if(i%4==0)putchar('\n');
        printf(i==0?"%f":", %f", ((Power*)gene)[i]);
    }
    printf("}\n");
}

void TrainRun()
{
    GAConfig config = malloc(sizeof(*config));
    config->ProbabilityOfHybrid = 0.25;
    config->ProbabilityOfVariation = 0.01;
    config->GetClone = GetClone;
    config->GetOneFitness = NULL;
    config->GetAllFitness = GetAllFitness;
    config->GetVariation = GetVariation;
    config->GetHybrid = GetHybrid;
    config->DeleteGene = DeleteGene;
    GAInitData init = malloc(sizeof(*init));
    init->MAXGenerations = 10;
    init->Config = config;
    init->ElitismCount = 1;
    int count = 4;
    GAGene starts[4];
    for (int i = 0; i < count; ++i) {
        starts[i] = GetVariation((Power*)AIPatternPowers_Default);
        printf("Start gene %d:\n",i);
        PrintGene(starts[i]);
    }
    init->StartGene = starts;
    init->GeneCount = count;
    GAGene re = GARun(init, 1);
    printf("Result: \n");
    PrintGene(re);
    free(config);
    free(init);
}