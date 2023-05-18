#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <string.h>
#include <iostream>
#include <string>
#include<mmstream.h>
#pragma comment(lib, "WINMM.LIB")

typedef struct play
{
    char name[5];//用户名
    int score;//用户历史最高分
    char pass[20];//用户的密码
    struct play* next;
}Player;

typedef struct car
{
    float x;//车的x坐标
    float y;//车的y坐标
    char name[5];
    int score;//当前分数
}car;

typedef struct ob
{
    int id;//障碍物的种类
    float x;//障碍物的x坐标
    float y;//障碍物的y坐标
    struct ob* next;
}Obj;

IMAGE tex[7],OBJ[7],bk;
car c1;
char usernow[5];
Obj * First = NULL;

Player* creat(FILE* fp, Player* head);//生成链表
Player* add(Player* head, char* n, char* s);//添加链表节点
void del(Player* head, char* n);//删除一个链表节点
Player* sort(Player* head);//链表排序(冒泡)
Player* Load(Player* head, FILE* fp);//加载
void stop(Player* head);//暂停界面
void car_move(Player* head);//检测按键小车移动
void showlist(Player* head);//展示排行榜
void game(Player* head);//游戏主程序
void generate(Player* head);//生成障碍物
Player* login(Player* head);//登录界面
void title(Player* head);//标题界面
Obj* crash_check(Player *head,Obj* m);//碰撞检测

int main()
{
    Player* h = NULL;
    FILE* fp;
    
    fp = fopen("./textures/data.dat", "r");
    h=Load(h,fp);
    fclose(fp);
    h=login(h);
    h = sort(h);
    title(h);
    system("pause");
    return 0;
    //初始化游戏界面，创建窗口并贴上贴图    
}

Player* creat(FILE* fp, Player* head)
{
    int n = 1;
    Player* p, * q;
    q = head;
    while (1)
    {
        p = (Player*)malloc(sizeof(Player));
        if (fscanf(fp, "%s%d%s", p->name, &p->score, p->pass) == EOF)//读取玩家信息，到EOF自动结束
        {
            free(p);//由于读到结尾，会有一个包含错误信息的节点
            break;
        };
        p->next = NULL;
        if (head == NULL)
            head = p;
        else
            q->next = p;
        q = p;
    }
    return head;
}

Player* add(Player* head, char* n, char* s)
{
    Player* p = NULL, * q = NULL;
    p = (Player*)malloc(sizeof(Player));
    strcpy(p->name, n);
    p->score = 0;
    strcpy(p->pass, s);
    p->next = NULL;
    if (head == NULL)
    {
        head = p;
    }
    else
    {
        q = head;
        while (q->next != NULL)
        {
            q = q->next;
        }
        q->next = p;
    }
    return head;
}

void del(Player* head, char* n)
{
    Player* p = head, * q;
    while (p->next->name == n)
    {
        p = p->next;
    }
    q = p->next;
    p->next = q->next;
    free(q);
}
void generate(Player *head)
{
    int a;
    Obj* p = NULL, * q = NULL;
    Obj *m;
    a = rand() % 3000;
    if (a >= 1 && a <= 3)
    {
        p = (Obj*)malloc(sizeof(Obj));
        p->x = 1024;
        p->y= rand() % 200 + 140;//生成在公路内
        p->next = NULL;
        p->id = a;
        if (First == NULL)
        {
            First = p;
            q = p;
        }
        else
        {
            for (q = First; q->next != NULL; q = q->next);
            q->next = p;
            q = q->next;
        }
    }
    if(First!=NULL)
    {
        for (m = First; m!= NULL; m = m->next)
        {
            if (m->x <= 0)
            {
                First = m->next;
                free(m);
                m = First;
                break;
            }
            else if(First !=NULL)
            {
                putimage(m->x, m->y, &OBJ[m->id+3], SRCAND);
                putimage(m->x, m->y, &OBJ[m->id], SRCINVERT);
                //EndBatchDraw();
                m->x -= 0.4;
                m=crash_check(head,m);
                if (m == NULL)
                    break;
            }
        }
        EndBatchDraw();
        return;
    }
}

Player* sort(Player* head)
{
    if (head == NULL || head->next == NULL)
        return head;
    Player* p = NULL;
    char tn[5] = { '\0' };
    char tp[20] = { '\0' };
    int ts=0;
    int change = 1;
    while (p != head->next && change==1)
    {
        Player* q = head;
        change = 0;
        for (; q->next && q->next != p; q = q->next)
        {
            if (q->score < q->next->score)
            {
                strcpy(tn, q->name);
                strcpy(q->name, q->next->name);
                strcpy(q->next->name, tn);
                strcpy(tp, q->pass);
                strcpy(q->pass, q->next->pass);
                strcpy(q->next->pass, tp);
                ts = q->score;
                q->score = q->next->score;
                q->next->score = ts;
                change = 1;
            }
        }
        p = q;
    }
    return head;
}

Player* Load(Player* head, FILE* fp)
{
    loadimage(&bk, "./textures/bk.png");
    loadimage(&tex[0], "./textures/Grass.png");
    loadimage(&tex[1], "./textures/Road1.png");
    loadimage(&tex[2], "./textures/Road2.png");
    loadimage(&tex[3], "./textures/car1.png", 71, 35);
    loadimage(&tex[4], "./textures/M.png", 71, 35);
    loadimage(&OBJ[1], "./textures/Barrel.png", 30,30);
    loadimage(&OBJ[4], "./textures/B.png", 30,30);
    loadimage(&OBJ[2], "./textures/coin.png", 30, 30);
    loadimage(&OBJ[5], "./textures/cion.png", 30, 30);
    loadimage(&OBJ[3], "./textures/Rock.png", 30, 30);
    loadimage(&OBJ[6], "./textures/st.png", 30, 30);
    //加载贴图资源
    mciSendString(TEXT("open ./textures/ms.mp3 alias mysong"), NULL, 0, NULL);
    mciSendString("play mysong repeat", 0, 0, 0);
    c1.y = 176;
    c1.x = 30;
    head = creat(fp, head);
    return head;
}

Obj* crash_check(Player* head, Obj* m)
{
    MOUSEMSG test;
    Player* p=NULL;
        if (fabs(c1.x - m->x) <= 50 && fabs(c1.y - m->y) <= 29)
        {
            if(m->id==1)//油桶
            {
                EndBatchDraw();
                setlinestyle(PS_SOLID, 10);
                setlinecolor(RGB(180, 180, 180));
                setfillcolor(RGB(210, 210, 210));;
                fillrectangle(402, 120, 622, 300.0);
                settextstyle(90, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(255, 153, 18));
                outtextxy(422, 140, "撞车");
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 128, 0));;
                fillrectangle(422, 330, 600, 360);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(250, 250, 250));
                outtextxy(452, 330, "退出游戏");
                while (1)
                {
                    FlushMouseMsgBuffer();
                    test = GetMouseMsg();
                    BeginBatchDraw();
                    if (test.x >= 422 && test.x <= 600 && test.y >= 330 && test.y <= 360)
                    {
                        m->x = 0;
                        setlinestyle(PS_SOLID, 3);
                        setlinecolor(RGB(255, 153, 18));
                        setfillcolor(RGB(255, 153, 0));;
                        fillrectangle(422, 330, 600, 360);
                        settextstyle(30, 0, "黑体");
                        setbkmode(TRANSPARENT);
                        settextcolor(RGB(255, 255, 255));
                        outtextxy(452, 330, "退出游戏");
                        if (test.uMsg == WM_LBUTTONDOWN)
                        {
                            setlinestyle(PS_SOLID, 3);
                            setlinecolor(RGB(255, 153, 18));
                            setfillcolor(RGB(255, 98, 0));;
                            fillrectangle(422, 330, 600, 360);
                            settextstyle(30, 0, "黑体");
                            setbkmode(TRANSPARENT);
                            settextcolor(RGB(200, 200, 200));
                            outtextxy(452, 330, "退出游戏");
                        }
                        if (test.uMsg == WM_LBUTTONUP)
                        {
                            p = head;
                            while (1)
                            {
                                if (strcmp(p->name, usernow) == 0)
                                    break;
                                p = p->next;
                            }
                            if (p->score < c1.score)
                                p->score = c1.score;
                            title(head);
                        }
                    }
                    else
                    {
                        setlinestyle(PS_SOLID, 3);
                        setlinecolor(RGB(255, 153, 18));
                        setfillcolor(RGB(255, 128, 0));;
                        fillrectangle(422, 330, 600, 360);
                        settextstyle(30, 0, "黑体");
                        setbkmode(TRANSPARENT);
                        settextcolor(RGB(250, 250, 250));
                        outtextxy(452, 330, "退出游戏");
                    }
                    EndBatchDraw();
                }
            }
            if (m->id == 2)//金币
            {
                c1.score+=3;
                First = m->next;
                free(m);
                m = First;
            }
            else if (m->id == 3)//石头
            {
                c1.score -= 2;
                First = m->next;
                free(m);
                m = First;
            }
    }
        return m;
}

void stop(Player* head)
{
    MOUSEMSG test;
    Player *p=NULL;
    EndBatchDraw();
    setlinestyle(PS_SOLID, 10);
    setlinecolor(RGB(180,180,180));
    setfillcolor(RGB(210,210,210));;
    fillrectangle(402, 120, 622, 400);
    settextstyle(90, 0, "黑体");
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 153, 18));
    outtextxy(422, 140, "暂停");

    setlinestyle(PS_SOLID, 3);
    setlinecolor(RGB(255, 153, 18));
    setfillcolor(RGB(255, 128, 0));;
    fillrectangle(422, 250, 600, 280);
    settextstyle(30, 0, "黑体");
    setbkmode(TRANSPARENT);
    settextcolor(RGB(250,250,250));
    outtextxy(452, 250, "继续游戏");

    setlinestyle(PS_SOLID, 3);
    setlinecolor(RGB(255, 153, 18));
    setfillcolor(RGB(255, 128, 0));;
    fillrectangle(422, 330, 600, 360);
    settextstyle(30, 0, "黑体");
    setbkmode(TRANSPARENT);
    settextcolor(RGB(250, 250, 250));
    outtextxy(452, 330, "退出游戏");
    while (1)
    {
        FlushMouseMsgBuffer();
        test = GetMouseMsg();
        BeginBatchDraw();
            if (test.x >= 422 && test.x <= 600 && test.y >= 250 && test.y <= 280)//鼠标悬停在按钮上
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 153, 0));;
                fillrectangle(422, 250, 600, 280);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(255,255,255));
                outtextxy(452, 250, "继续游戏");
                if (test.uMsg == WM_LBUTTONDOWN)//按下左键
                {
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 153, 18));
                    setfillcolor(RGB(255, 98, 0));;
                    fillrectangle(422, 250, 600, 280);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(200, 200, 200));
                    outtextxy(452, 250, "继续游戏");
                }
                if (test.uMsg == WM_LBUTTONUP)//抬起左键
                {
                    BeginBatchDraw();
                    return;
                }
            }
            else
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 128, 0));;
                fillrectangle(422, 250, 600, 280);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(250, 250, 250));
                outtextxy(452, 250, "继续游戏");
            }
            if (test.x >= 422 && test.x <= 600 && test.y >= 330 && test.y <= 360)
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 153, 0));;
                fillrectangle(422, 330, 600, 360);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(255,255,255));
                outtextxy(452, 330, "退出游戏");
                if (test.uMsg == WM_LBUTTONDOWN)
                {
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 153, 18));
                    setfillcolor(RGB(255, 98, 0));;
                    fillrectangle(422, 330, 600, 360);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(200,200,200));
                    outtextxy(452, 330, "退出游戏");
                }
                if (test.uMsg == WM_LBUTTONUP)
                {
                    p = head;
                    while (1)
                    {
                        if (strcmp(p->name, usernow) == 0)
                            break;
                        p = p->next;
                    }
                    if (p->score < c1.score)
                        p->score = c1.score;
                    title(head);
                    
                }
            }
            else
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 128, 0));
                fillrectangle(422, 330, 600, 360);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(250, 250, 250));
                outtextxy(452, 330, "退出游戏");
            }
            EndBatchDraw();
    }
}

void car_move(Player* head)
{
    if (GetAsyncKeyState(VK_UP))
    {
        if (c1.y > 132)
            c1.y = c1.y - 0.17;
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        if (c1.y < 346)
            c1.y = c1.y + 0.17;
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        if (c1.x >= 20)
            c1.x = c1.x - 0.1;
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        if (c1.x < 800)
            c1.x = c1.x + 0.15;
    }
    if (c1.x > 30)
        c1.x = c1.x - 0.05;
    if (GetAsyncKeyState(VK_ESCAPE))
    {
        stop(head);
    }
}

void showlist(Player* head)
{

    int i = 1, j = 0;
    int a[10] = { 0 };
    char list[1000] = { '\0' };
    Player* p;
    p = head;
    if (head == NULL)
    {
        MessageBox(NULL, list, "排行榜", MB_SETFOREGROUND);
        return;
    }
    head=sort(head);
    while (p != NULL && i < 11)
    {
        j += sprintf(list + j, "%d\t%s\t%d\n", i, p->name, p->score);
        p = p->next;
        i++;
    }
    MessageBox(NULL, list, "排行榜", MB_SETFOREGROUND);
    return;
}

void game(Player* head)
{
    MOUSEMSG test;
    Player* p = head;
    float i, j,t=0;
    int time = 50000;
    char score[20] = { '\0' };
    char high[20] = { '\0' };
    char T[6]={'\0'};
    j = 0;
    while (1)
    {
        if (strcmp(p->name, usernow) == 0)
            break;
        p = p->next;
    }
    _itoa(p->score, high, 10);
    initgraph(1024, 512);
    c1.score = 0;
    c1.y = 176;
    c1.x = 30;
    while (1)
    {
        BeginBatchDraw();
        for (i = 0; i <= 2048; i += 256)
        {
            putimage(i - j, 0, &tex[0]);
            putimage(i - j, 128, &tex[1]);
            putimage(i - j, 256, &tex[2]);
            putimage(i - j, 384, &tex[0]);
        }
        putimage(c1.x, c1.y, &tex[3], SRCAND);
        putimage(c1.x, c1.y, &tex[4], SRCINVERT);
        _itoa(c1.score,score,10);
        _itoa(time/100, T, 10);
        settextstyle(30, 0, "黑体");
        setbkmode(TRANSPARENT);
        settextcolor(RGB(255, 255, 255));
        outtextxy(900, 50, score);
        outtextxy(700, 50, T);
        outtextxy(500, 50, high);
        outtextxy(300, 50, p->name);
        outtextxy(800, 50, "score:");
        outtextxy(600, 50, "time:");
        outtextxy(400, 50, "high:");
        outtextxy(200, 50, "name:");

        car_move(head);
        generate(head);
        EndBatchDraw();
        Sleep(0.7);
        time = time - 1;
        j += 0.4;
        if (j >= 1024)
        {
            j = 0;
            c1.score++;
        }
        if (time <= 0)
        {
            EndBatchDraw();
            setlinestyle(PS_SOLID, 10);
            setlinecolor(RGB(180, 180, 180));
            setfillcolor(RGB(210, 210, 210));;
            fillrectangle(362, 120, 662, 300.0);
            settextstyle(90, 0, "黑体");
            setbkmode(TRANSPARENT);
            settextcolor(RGB(255, 153, 18));
            outtextxy(382, 140, "时间到");
            setlinestyle(PS_SOLID, 3);
            setlinecolor(RGB(255, 153, 18));
            setfillcolor(RGB(255, 128, 0));;
            fillrectangle(422, 330, 600, 360);
            settextstyle(30, 0, "黑体");
            setbkmode(TRANSPARENT);
            settextcolor(RGB(250, 250, 250));
            outtextxy(452, 330, "退出游戏");
            while (1)
            {
                FlushMouseMsgBuffer();
                test = GetMouseMsg();
                BeginBatchDraw();
                if (test.x >= 422 && test.x <= 600 && test.y >= 330 && test.y <= 360)
                {
                    First->x = 0;
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 153, 18));
                    setfillcolor(RGB(255, 153, 0));;
                    fillrectangle(422, 330, 600, 360);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(255, 255, 255));
                    outtextxy(452, 330, "退出游戏");
                    if (test.uMsg == WM_LBUTTONDOWN)
                    {
                        setlinestyle(PS_SOLID, 3);
                        setlinecolor(RGB(255, 153, 18));
                        setfillcolor(RGB(255, 98, 0));;
                        fillrectangle(422, 330, 600, 360);
                        settextstyle(30, 0, "黑体");
                        setbkmode(TRANSPARENT);
                        settextcolor(RGB(200, 200, 200));
                        outtextxy(452, 330, "退出游戏");
                    }
                    if (test.uMsg == WM_LBUTTONUP)
                    {
                        if (p->score < c1.score)
                            p->score = c1.score;
                        title(head);
                    }
                }
                else
                {
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 153, 18));
                    setfillcolor(RGB(255, 128, 0));;
                    fillrectangle(422, 330, 600, 360);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(250, 250, 250));
                    outtextxy(452, 330, "退出游戏");
                }
                EndBatchDraw();
            }
        }
    }
}

Player* login(Player* head)
{
    Player* p;
    int i=3;
    char user[5] = { '\0' }, password[20] = { '\0' },empty[5]={'\0'};
    do
    {
        InputBox(user, 5, "请输入用户名，最多五位，若为新用户则自动注册", "登录");
    } while (strcmp(user,empty)==0);
    for (p = head; p != NULL; p = p->next)
    {
        if (strcmp(p->name, user) == 0)
        {
            do
            {
                InputBox(password, 20, "请输入密码，最多20位", "登录");
                if (strcmp(p->pass, password) == 0)
                {
                    strcpy(c1.name, user);
                    strcpy(usernow,user);
                    return head;
                }
                else
                {
                    if(i>0)
                    {
                        MessageBox(NULL, "密码错误，请再次尝试", "登录", MB_SETFOREGROUND);
                        i--;
                    }
                    else
                    {
                        MessageBox(NULL, "错误次数过多，游戏退出", "登录", MB_SETFOREGROUND);
                        exit(0);
                    }
                }
            } while ((strcmp(p->pass, password) != 0));
        }
    }
    if (p == NULL)
    {
        InputBox(password, 20, "请输入密码，最多20位", "注册");
        head = add(head, user, password);
        strcpy(c1.name , user);
        strcpy(usernow, user);
        return head;
    }
}

void title(Player* head)
{
    initgraph(1080, 720);
    putimage(0, 0, &bk);
    settextstyle(90, 0, "黑体");
    setbkmode(TRANSPARENT);
    settextcolor(RGB(190, 190, 190));
    outtextxy(360, 100, "赛车游戏");
    //初始化标题界面
    FILE* fp;
    Player* p=NULL;
    MOUSEMSG test;
    test = GetMouseMsg();
    while (true)
    {
        FlushMouseMsgBuffer();
        
        BeginBatchDraw();
            if (test.x >= 480 && test.x <= 600 && test.y >= 330 && test.y <= 360)
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(235, 153, 0));;
                fillrectangle(480, 330, 600, 360);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(255,255,255));
                outtextxy(480, 330, "开始游戏");
                if (test.uMsg == WM_LBUTTONDOWN)
                {
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 153, 18));
                    setfillcolor(RGB(255, 98, 0));;
                    fillrectangle(480, 330, 600, 360);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(200,200,200));
                    outtextxy(480, 330, "开始游戏");
                }
                if (test.uMsg == WM_LBUTTONUP)
                {
                    MessageBox(NULL, "进入游戏后使用方向键控制车移动，按下esc暂停，撞到障碍物触发事件：\n金币：加3分\n石头：扣两分\n油桶：游戏结束", "游戏玩法", MB_SETFOREGROUND);
                    game(head);
                }
            }
            else
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 128, 0));;
                fillrectangle(480, 330, 600, 360);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(250, 250, 250));
                outtextxy(480, 330, "开始游戏");
            }
            if (test.x >= 480 && test.x <= 600 && test.y >= 390 && test.y <= 420)
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(235, 153, 0));
                fillrectangle(480, 390, 600, 420);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(255,255,255));
                outtextxy(495, 390, "排行榜");
                if (test.uMsg == WM_LBUTTONDOWN)
                {
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 98, 18));
                    setfillcolor(RGB(255, 128, 0));
                    fillrectangle(480, 390, 600, 420);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(200,200,200));
                    outtextxy(495, 390, "排行榜");
                }
                if (test.uMsg == WM_LBUTTONUP)
                   showlist(head);
            }
            else
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 128, 0));
                fillrectangle(480, 390, 600, 420);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(250, 250, 250));
                outtextxy(495, 390, "排行榜");
            }
            if (test.x >= 480 && test.x <= 600 && test.y >= 450 && test.y <= 480)
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(235, 153, 0));;
                fillrectangle(480, 450, 600, 480);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(255,255,255));
                outtextxy(480, 450, "退出游戏");
                if (test.uMsg == WM_LBUTTONDOWN)
                {
                    setlinestyle(PS_SOLID, 3);
                    setlinecolor(RGB(255, 153, 18));
                    setfillcolor(RGB(255, 98, 0));;
                    fillrectangle(480, 450, 600, 480);
                    settextstyle(30, 0, "黑体");
                    setbkmode(TRANSPARENT);
                    settextcolor(RGB(200,200,200));
                    outtextxy(480, 450, "退出游戏");
                }
                if (test.uMsg == WM_LBUTTONUP)
                {
                    closegraph();
                    fp = fopen(".\\textures\\data.dat", "w");
                    head=sort(head);
                    for (; head != NULL; head = head->next)
                        fprintf(fp, "%s %d %s\n", head->name, head->score, head->pass);
                    fclose(fp);
                    exit(0);
                }
            }
            else
            {
                setlinestyle(PS_SOLID, 3);
                setlinecolor(RGB(255, 153, 18));
                setfillcolor(RGB(255, 128, 0));;
                fillrectangle(480, 450, 600, 480);
                settextstyle(30, 0, "黑体");
                setbkmode(TRANSPARENT);
                settextcolor(RGB(250, 250, 250));
                outtextxy(480, 450, "退出游戏");
            }
           EndBatchDraw();
           test = GetMouseMsg();
    }
}
