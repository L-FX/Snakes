#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

// 蛇的状态，U：上 ；D：下；L:左 R：右
#define U 1
#define D 2
#define L 3
#define R 4

// 在全局变量中添加一个结构体用于保存用户信息
typedef struct User
{
    char username[20];
    char password[20];
} user;

typedef struct SNAKE      // 蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE *next;
} snake;

// 全局变量//
int score = 0, add = 10;    // 总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake *head, *food;        // 蛇头指针，食物指针
snake *q;                   // 遍历蛇的时候用到的指针
int endgamestatus = 0;      // 游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。
int currentUserIndex = -1;  // 当前登录用户的索引，初始值为-1表示无用户登录

// 全局变量中定义一个用户数组：
user users[10];    // 最多支持 10 个用户
int userCount = 0; // 用户数量

// 声明全部函数//
void registerUser(); // 声明注册函数
int loginUser();     // 声明登录函数
void Pos();
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();

// 添加一个注册函数和登录函数：
void registerUser()
{
    char tempUsername[20]; // 限制用户名长度为 20
    char tempPassword[20]; // 限制密码长度为 20

    printf("请输入用户名（最多 20 个字符）：");
    scanf("%19s", tempUsername); // 读取最多 19 个字符，留一个位置给终止字符'\0'
    strncpy(users[userCount].username, tempUsername, 19); // 将读取到的用户名拷贝到 users 数组
    users[userCount].username[19] = '\0'; // 手动添加终止字符

    printf("请输入密码（最多 20 个字符）：");
    scanf("%19s", tempPassword); // 读取最多 19 个字符，留一个位置给终止字符'\0'
    strncpy(users[userCount].password, tempPassword, 19); // 将读取到的密码拷贝到 users 数组
    users[userCount].password[19] = '\0'; // 手动添加终止字符

    userCount++;
    printf("注册成功！\n");
}
int loginUser()
{
    char username[20];
    char password[20];
    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
        {
            printf("登录成功！\n");
            currentUserIndex = i;  // 记录当前登录用户的索引
            return 1; // 登录成功
        }
    }
    printf("用户名或密码错误！\n");
    return 0; // 登录失败
}

// 优化：添加了颜色设置
void Pos(int x, int y)
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
    // 得分使用了绿色，食物得分使用了黄色
    if (x == 64 && y == 11)
    {
        printf("\033[32m得分：%d  ", score);
    }
    else if (x == 64 && y == 10)
    {
        printf("\033[33m每个食物得分：%d 分", add);
    }
    else
    {
        printf(" ");
    }
}

void creatMap()
{
    int i;
    for (i = 0; i < 58; i += 2)
    {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++)
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake()
{
    snake *tail;
    int i;
    tail = (snake *)malloc(sizeof(snake));
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake *)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail!= NULL)
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()
{
    snake *self;
    self = head->next;
    while (self!= NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood()
{
    snake *food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake *)malloc(sizeof(snake));
    while ((food_1->x % 2)!= 0)
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    q = head;
    while (q->next == NULL)
    {
        if (q->x == food_1->x && q->y == food_1->y)
        {
            free(food_1);
            createfood();
        }
        q = q->next;
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}

void cantcrosswall()
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()
{
    snake *nexthead;
    cantcrosswall();

    nexthead = (snake *)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next!= NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

void gamecircle()
{

    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);

    // 在这里添加显示当前登录用户信息的代码
    if (currentUserIndex!= -1)
    {
        Pos(64, 19);
        printf("***%s 正在游戏中***", users[currentUserIndex].username);
    }

    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d 分", add);
    }
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d 分", add);
        if (GetAsyncKeyState(VK_UP) && status!= D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status!= U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status!= R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status!= L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到 1 之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //保证最低分为 1
                }
            }
        }
        Sleep(sleeptime);
        snakemove();
    }
}

void welcometogame()
{
    Pos(40, 12);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("用↑.↓.←.→分别控制蛇的移动， F1 为加速，2 为减速\n");
    Pos(25, 13);
    printf("加速将能得到更高的分数。\n");
    system("pause");
    system("cls");
}

void endgame()
{

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    exit(0);
}
// 在 gamestart() 函数中调用注册函数和登录函数：
// 修改后的 gamestart() 函数
void gamestart()
{
    system("mode con cols=100 lines=30");

    while (1)
    {
        int choice;
        printf("欢迎来到贪食蛇游戏！\n");
        printf("1. 注册\n2. 登录\n3. 退出\n");
        scanf("%d", &choice);

        if (choice == 1)
        {
            registerUser();
        }
        else if (choice == 2)
        {
            if (loginUser())
            {
                break; // 登录成功，退出循环，开始游戏
            }
        }
        else if (choice == 3)
        {
            exit(0);  // 退出游戏
        }
        system("cls");
    }

    welcometogame(); // 欢迎信息
    creatMap();      // 创建游戏地图
    initsnake();     // 初始化蛇
    createfood();    // 创建食物
}

int main()
{
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
