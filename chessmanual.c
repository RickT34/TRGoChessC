#include "chessmanual.h"
#include <stdio.h>

struct patternlib {
    int *patterns;         // b11111, b11110, ...
    int *patternsSize; // 5, 5, ...
    int *powers;
    int size;
};

static int LineKeys[1 << 10];
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

void ChessManualInit() { 
    countKeys(0, 0); 
    #ifdef DEBUG
    printfD("Count Keys Success, line keys: %d\n",keysCount);
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

Key ChessTableXRowToKey(ChessTable ct, int x){
    x=GetPoint(x, 0);
    ct+=x;
    Key k=0;
    Line ret;
    for(int i=0;i<3;i++){
        ret=0;
        for(int j=0;j<5;j++, ct+=YSTEP){
            ret|=(*ct)<<(j*2);
        }
        #ifdef DEBUG
        printfD("LinePart: %d\n", i);
        printLine(ret, 5);
        #endif
        k=(k<<8)|LineKeys[ret];
    }
    return k;
}

Key ChessTableYRowToKey(ChessTable ct, int y){
    y=GetPoint(0, y);
    ct+=y;
    Key k=0;
    Line ret;
    for(int i=0;i<3;i++){
        ret=0;
        for(int j=0;j<5;j++, ct+=XSTEP){
            ret|=(*ct)<<(j*2);
        }
        #ifdef DEBUG
        printfD("LinePart: %d\n", i);
        printLine(ret, 5);
        #endif
        k=(k<<8)|LineKeys[ret];
    }
    return k;
}

Key ChessTableDRowToKey(ChessTable ct, int s, Player wall){
    Line ret;
    ret=Min(s, 14);
    s=GetPoint(ret, s-ret);
    ret=s-14;
    int step=16-Abs(ret);
    ct+=s-DSTEP;
    Key k=0;
    for(int i=0;i<3;i++){
        ret=0;
        for(int j=0;step&&j<5;j++, step--){
            if(step==1){
                ret|=wall<<(j*2);
            }
            else{
                ct+=DSTEP;
                ret|=(*ct)<<(j*2);
            }
        }
        #ifdef DEBUG
        printfD("LinePart: %d\n", i);
        printLine(ret, 5);
        #endif
        k=(k<<8)|LineKeys[ret];
    }
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
ChessManual ChessManualMake(PatternLib pattern) {
    ChessManual re = malloc(sizeof(int) * (1<<24));
    #ifdef DEBUG
    printfD("Start...\n");
    #endif
    countLib(re, 0, 0, pattern);
    #ifdef DEBUG
    printfD("Success\n");
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

void printLine(Line line,int n){
    for(int i=0;i<n;i++,line>>=2)
        printf("%d",line&3);
    printf("\n");
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
    printfD("Success, size: %d\n",size);
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