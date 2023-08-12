#include "chessmanual.h"
#define Merge10bit(array, iv, moveiv, ret)  ret=(*array)<<(moveiv); array+=iv;\
                                            ret|=(*array)<<(2+moveiv); array+=iv;\
                                            ret|=(*array)<<(4+moveiv); array+=iv;\
                                            ret|=(*array)<<(6+moveiv); array+=iv;\
                                            ret|=(*array)<<(8+moveiv); array+=iv

#define ChessTableToKey(array, iv, k)   int _ret;\
                                        Merge10bit(array, iv, 0, _ret);\
                                        k=LineKeys[_ret];\
                                        Merge10bit(array, iv, 10, _ret);\
                                        k|=LineKeys[_ret]<<8;\
                                        Merge10bit(array, iv, 20, _ret);\
                                        k|=LineKeys[_ret]<<16;

struct patternlib {
    int *patterns;         // b11111, b11110, ...
    int *patternsSize; // 5, 5, ...
    int *powers;
    int size;
};

static int LineKeys[1 << 10];
static char inited = 0;
static int keysCount = 0;

void countKeys(Key line, int count) {
    if (count == 5) {
        LineKeys[line] = keysCount++;
        return;
    }
    countKeys((line << 2), count + 1);
    countKeys((line << 2) | 1, count + 1);
    countKeys((line << 2) | 2, count + 1);
}

int countPower(Line line, PatternLib pattern) {
    int power = 0, mask, p;
    for (int i = 0; i < pattern->size; i++) {
        mask = (1 << (pattern->patternsSize[i] * BIT)) - 1;
        p = pattern->patterns[i];
        for (int j = 0; j <= LLN - pattern->patternsSize[i];
                j++, mask <<= BIT, p <<= BIT) {
            if ((line & mask) == p)
                power += pattern->powers[i];
        }
    }
    return power;
}

void init() { 
    countKeys(0, 0); 
    #ifdef DEBUG
    printfD("Count Keys Success, line keys: %d",keysCount);
    #endif
}

Key LineToKey(Line line) {
    Key k=LineKeys[line&1023];
    line>>=10;
    k |= LineKeys[line&1023]<<8;
    line>>=10;
    k |= LineKeys[line&1023]<<16;
    return k;
}

Key ChessTableToKeyX(ChessTable ct, int x){
    x=GetPoint(x, 0);
    ct+=x;
    Key k=0;
    ChessTableToKey(ct, XSTEP, k);
    return k;
}

void countLib(ChessManual manual, Line line, int count, PatternLib pattern) {
    if (count == LLN) {
        int k = LineToKey(line);
        manual[k] = countPower(line, pattern);
        return;
    }
    countLib(manual, (line << BIT), count + 1, pattern);
    countLib(manual, (line << BIT) | PlayerW, count + 1, pattern);
    countLib(manual, (line << BIT) | PlayerB, count + 1, pattern);
}

// Power XXXXX=4096, XXXXO=32, XXXO=15, XXOXO=14, XXO=1
ChessManual MakeLib(PatternLib pattern) {
    if (!inited)
        init();
    ChessManual re = malloc(sizeof(int) * (1<<24));
    #ifdef DEBUG
    printfD("Start...");
    #endif
    countLib(re, 0, 0, pattern);
    #ifdef DEBUG
    printfD("Success");
    #endif
    return re;
}

Line StrToLine(char *s) {
    int re = 0;
    while (*s) {
        re = (re << BIT) | (*s - '0');
        ++s;
    }
    return re;
}

PatternLib CompilePatterns(char **patterns, int *powers, int size) {
    int *pa = malloc(sizeof(int) * size), *paS = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        pa[i] = 0;
        char *t = patterns[i] - 1;
        while (*(++t)) {
            pa[i] = (pa[i] << BIT) + *t - '0';
        }
        paS[i] = t - patterns[i];
    }
    PatternLib re = malloc(sizeof(struct patternlib));
    re->patternsSize = paS;
    re->patterns = pa;
    re->powers = powers;
    re->size = size;
    #ifdef DEBUG
    printfD("Success, size: %d",size);
    #endif
    return re;
}
/*
int main() {
    //                 11111,11110,01111,0111,1110,11010,01011,110,011
    int pattern[] = {31, 30, 15, 7, 14, 26, 11, 6, 3},
            patternSize[] = {5, 5, 5, 4, 4, 5, 5, 3, 3},
            powers[] = {16384, 32, 32, 15, 15, 14, 14, 1, 1}, size = 9;
    char *pt[] = {"11111", "11110", "01111", "0111", "1110",
                                "11010", "01011", "110",     "011"};
    PatternLib p = CompilePatterns(pt, powers, size);
    ChessManual c = MakeLib(p);
    // 110110011101111
    char s[] = "011120111102211";
    printf("%d\n", c[LineToKey(StrToLine(s))]);
    return 0;
}*/