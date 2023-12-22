#ifndef _GAH
#define _GAH
/**********************遗传算法相关*************************/
typedef double GAScore; // 适应度分数
typedef void *GAGene;   // 基因

typedef struct
{
    double ProbabilityOfVariation;                                    // 变异概率
    double ProbabilityOfHybrid;                                       // 交叉互换概率
    GAScore (*GetOneFitness)(const GAGene ind);                       // 计算基因适应度分数
    GAScore *(*GetAllFitness)(const GAGene *allind, const int count); // 计算每个基因的适应度分数，便于并行计算实现，当该参数不为NULL时优先使用该方法
    GAGene (*GetClone)(const GAGene ind);                             // 获取基因克隆
    GAGene (*GetVariation)(const GAGene ind);                         // 获取基因变异结果
    GAGene (*GetHybrid)(const GAGene ind1, const GAGene ind2);        // 获取两个基因交叉互换结果
    void (*DeleteGene)(GAGene ind);                                   // 释放基因
    void (*PrintGene)(const GAGene ind);                              // 打印基因，如果display启用
} *GAConfig;                                                          // 遗传算法参数

typedef struct
{
    int MAXGenerations; // 最大迭代次数
    GAConfig Config;    // 遗传算法参数
    GAGene *StartGene;  // 初始基因种群
    int GeneCount;      // 初始基因种群个体数
    int ElitismCount;   // 精英主义参数，即每次迭代保留的上代最好个体数
} *GAInitData;          // 遗传算法初始化数据

/// @brief 启动遗传算法
/// @param init 遗传算法初始化数据
/// @param display 是否显示过程
/// @return 优化结果
GAGene GARun(GAInitData init, int display);

#endif