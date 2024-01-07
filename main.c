#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define TrainMode
// #define PKMode
#define PKAI0 AIPatternPowers_Default_G4
#define PKAI1 AIPatternPowers_Default_G7

char buff[4096];
#define BUFFSIZE 4096
void MakeUI(Game game)
{
    // system("clear");
    printf("回合: %d\n", (game->history->Count + 1) / 2);
    PrintChessBoard(game->chessboard, ChessBoardUseStyle);
    printf("当前: ");
    PrintPlayer(game, GameNextPlayerID(game->nowPlayerID));
    putchar('\n');
    printf("输入 '?' 查看帮助。\n");
}
void MakeProcessBar(int p, int len, const int width)
{ // p in [1,len]
    putchar('[');
    char last = '[';
    for (int i = 1; i <= width; ++i)
    {
        if (i * len < p * width)
            putchar(last = '=');
        else
        {
            if (last == '=')
                putchar(last = '>');
            else
                putchar('-');
        }
    }
    putchar(']');
    printf(" (%d/%d)\n", p, len);
}

void StartGameRecord(Game game)
{
    printf("复盘开始。\n");
    ChessBoard re;
    int flame = game->history->Count;
    GameRecord gr = NewGameRecord(game);
    do
    {
        printf("输入数字来查看指定帧，输入 (换行) 或'+'查看下一帧，'-'查看上一帧。输入'q'退出。\n");
        re = GameRecordRead(gr, flame - 1);
        if (re != NULL)
        {
            printf("帧: %d\n", flame);
            PrintChessBoard(re, ChessBoardUseStyle);
            MakeProcessBar(flame, gr->datalen, 30);
            FreeChessBoard(re);
            re = NULL;
        }
        else
        {
            if (flame > 1)
                flame = game->history->Count;
            else
                flame = 1;
        }
        Input(buff, BUFFSIZE);
        if (buff[0] == '+' || buff[0] == '\n')
            flame += 1;
        else if (buff[0] == '-')
            flame -= 1;
        else
            sscanf(buff, "%d", &flame);
    } while (buff[0] != 'q');
    MakeUI(game);
}

Point GetPointInput(const Game game, const char *s)
{
    int x, y;
    char c, c2;
    int ok = 0;
    if (sscanf(s, "%d%c", &y, &c) == 2)
    {
        if (c >= 'A' && c <= 'Z')
            x = c - 'A';
        else
            x = c - 'a';
        y -= 1;
        ok = 1;
    }
    else if (sscanf(s, "%c%d", &c, &y) == 2)
    {
        if (c >= 'A' && c <= 'Z')
            x = c - 'A';
        else
            x = c - 'a';
        y -= 1;
        ok = 1;
    }
    if (!IsLegalXY(x, y))
        ok = 0;
    if (GetChessXY(game->chessboard, x, y) != BLANK)
        ok = 0;
    if (!ok)
        return PointNULL;
    return GetPoint(x, y);
}

int InputCommamd(Game game, char *buff)
{
    if (buff[0] == '-')
    {
        GameUndo(game);
        if (GameGetNextPlayer(game)->type == PlayerType_AI)
            GameUndo(game);
        MakeUI(game);
        return 0;
    }
    else if (buff[0] == '>')
    {
        char file[4096];
        file[0] = '|';
        int l = GameSave(game, file + 1);
        file[1 + l] = '|';
        file[2 + l] = 0;
        printf("保存成功，请保存这段字符串: ");
        puts(file);
        return 0;
    }
    else if (buff[0] == 'r')
    {
        StartGameRecord(game);
        return 0;
    }
    else if (buff[0] == '<')
    {
        Game loadgame;
        int i = 0;
        while (buff[i] != '|' && i < BUFFSIZE)
        {
            ++i;
        }
        if (i < BUFFSIZE)
        {
            GameLoad(&loadgame, buff + i + 1);
            printf("读取成功!\n");
            Start(loadgame);
            FreeGame(loadgame);
            printf("正在返回先前对局!\n");
            MakeUI(game);
        }
        else
        {
            printf("读取失败!\n");
        }

        return 0;
    }
    else if (buff[0] == 'q')
    {
        return 2;
    }
    else if (buff[0] == '?')
    {
        printf("-\t悔棋\n>\t保存\n<\t读取\nr\t复盘\nq\t退出\n");
        return 0;
    }
    return 1;
}

void PrintPlayer(Game game, int id)
{
    printf("%s [%s]", GetChessSkin(PlayerChessTypes[id], ChessBoardStyle_Classic),
           game->players[id]->name);
}

void Start(Game game)
{
    int ret;
    int comret = 1;
    do
    {
        if (comret)
            MakeUI(game);
        if (IsGameStopped(game))
        {
            if (game->status == GameStatus_End)
            {
                PrintPlayer(game, game->nowPlayerID);
                printf(" 赢了!\n");
            }
            printf("游戏结束!\n");
            Input(buff, BUFFSIZE);
            comret = InputCommamd(game, buff);
            if (comret == 2)
                return;
        }
        else
        {
            if (GameGetNextPlayer(game)->type == PlayerType_Human)
            {
                Point p = PointNULL;
                while (p == PointNULL)
                {
                    Input(buff, BUFFSIZE);
                    if (comret = InputCommamd(game, buff))
                    {
                        if (comret == 2)
                            return;
                        p = GetPointInput(game, buff);
                        if (p == PointNULL)
                            printf("非法位置，请重试!\n");
                    }
                }
                *(Point *)GameGetNextPlayer(game)->data = p;
            }
#ifdef DEBUG
            else
            {
                printf("Continue?");
                getchar();
            }
#endif
            ret = GameNextTurn(game);
            PrintPlayer(game, game->nowPlayerID);
            printf(" 落子于 [%d%c]\n", PointTo2C(((Action)StackTop(game->history))->point));
        }
    } while (1);
}

int Run()
{
    Player p[2];
    p[0] = p[1] = NULL;
    printf("欢迎!\n输入 'q' 退出:\n");
    for (int i = 0; i < 2; ++i)
    {
        printf("\n请选择");
        printf(i == 0 ? "先手玩家:\n" : "后手玩家:\n");
        printf("[1]: 人类玩家\n[2]: 电脑玩家\n\n请选择: ");
        Input(buff, BUFFSIZE);
        if (buff[0] == '1')
        {
            p[i] = NewHumanPlayer(PlayerNames[0]);
        }
        else if (buff[0] == '2')
        {
            p[i] = NewAIPlayer(PlayerNames[1], i, GameUseAIPower);
        }
        else if (buff[0] == 'q')
        {
            return 1;
        }
        else
        {
            printf("请重试!\n");
            --i;
        }
    }
    if (p[0] != NULL && p[1] != NULL)
    {
        Game game = NewGame(p[0], p[1]);
        Start(game);
        FreeGame(game);
    }
    for (int i = 0; i < 2; ++i)
    {
        if (p[i] != NULL)
        {
            if (p[i]->type == PlayerType_Human)
            {
                FreeHumanPlayer(p[i]);
            }
            else
            {
                FreeAIPlayer(p[i]);
            }
        }
    }
    return 0;
}

int main(int args, char **argv)
{
    // test();
    printf("初始化中...\n");
    ChessBoardInit();
    GameManagerInit();
    // while(1){
    //     int x,y;
    //     scanf("%d%d",&x,&y);
    //     PrintPointInf(CBINF,GetPoint(x,y));
    // }
#ifdef TrainMode
    TrainRun();
    Input(buff, BUFFSIZE);
#endif
#ifdef PKMode
    Player p1, p2;
    p1 = NewAIPlayer("AI0", 0, PKAI0);
    p2 = NewAIPlayer("AI1", 1, PKAI1);
    Game game = NewGame(p1, p2);
    printf("0 first:\n");
    Start(game);
    FreeGame(game);
    FreeAIPlayer(p1);
    FreeAIPlayer(p2);
    p2 = NewAIPlayer("AI0", 1, PKAI0);
    p1 = NewAIPlayer("AI1", 0, PKAI1);
    game = NewGame(p1, p2);
    printf("1 first:\n");
    Start(game);
    FreeGame(game);
    FreeAIPlayer(p1);
    FreeAIPlayer(p2);
#endif
    while (!Run())
        ;

    return 0;
}