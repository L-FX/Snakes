#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

// �ߵ�״̬��U���� ��D���£�L:�� R����
#define U 1
#define D 2
#define L 3
#define R 4

// ��ȫ�ֱ��������һ���ṹ�����ڱ����û���Ϣ
typedef struct User
{
    char username[20];
    char password[20];
} user;

typedef struct SNAKE      // �����һ���ڵ�
{
    int x;
    int y;
    struct SNAKE *next;
} snake;

// ȫ�ֱ���//
int score = 0, add = 10;    // �ܵ÷���ÿ�γ�ʳ��÷֡�
int status, sleeptime = 200;//ÿ�����е�ʱ����
snake *head, *food;        // ��ͷָ�룬ʳ��ָ��
snake *q;                   // �����ߵ�ʱ���õ���ָ��
int endgamestatus = 0;      // ��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��
int currentUserIndex = -1;  // ��ǰ��¼�û�����������ʼֵΪ-1��ʾ���û���¼

// ȫ�ֱ����ж���һ���û����飺
user users[10];    // ���֧�� 10 ���û�
int userCount = 0; // �û�����

// ����ȫ������//
void registerUser(); // ����ע�ắ��
int loginUser();     // ������¼����
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

// ���һ��ע�ắ���͵�¼������
void registerUser()
{
    char tempUsername[20]; // �����û�������Ϊ 20
    char tempPassword[20]; // �������볤��Ϊ 20

    printf("�������û�������� 20 ���ַ�����");
    scanf("%19s", tempUsername); // ��ȡ��� 19 ���ַ�����һ��λ�ø���ֹ�ַ�'\0'
    strncpy(users[userCount].username, tempUsername, 19); // ����ȡ�����û��������� users ����
    users[userCount].username[19] = '\0'; // �ֶ������ֹ�ַ�

    printf("���������루��� 20 ���ַ�����");
    scanf("%19s", tempPassword); // ��ȡ��� 19 ���ַ�����һ��λ�ø���ֹ�ַ�'\0'
    strncpy(users[userCount].password, tempPassword, 19); // ����ȡ�������뿽���� users ����
    users[userCount].password[19] = '\0'; // �ֶ������ֹ�ַ�

    userCount++;
    printf("ע��ɹ���\n");
}
int loginUser()
{
    char username[20];
    char password[20];
    printf("�������û�����");
    scanf("%s", username);
    printf("���������룺");
    scanf("%s", password);
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
        {
            printf("��¼�ɹ���\n");
            currentUserIndex = i;  // ��¼��ǰ��¼�û�������
            return 1; // ��¼�ɹ�
        }
    }
    printf("�û������������\n");
    return 0; // ��¼ʧ��
}

// �Ż����������ɫ����
void Pos(int x, int y)
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
    // �÷�ʹ������ɫ��ʳ��÷�ʹ���˻�ɫ
    if (x == 64 && y == 11)
    {
        printf("\033[32m�÷֣�%d  ", score);
    }
    else if (x == 64 && y == 10)
    {
        printf("\033[33mÿ��ʳ��÷֣�%d ��", add);
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
        printf("��");
        Pos(i, 26);
        printf("��");
    }
    for (i = 1; i < 26; i++)
    {
        Pos(0, i);
        printf("��");
        Pos(56, i);
        printf("��");
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
        printf("��");
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
    printf("��");
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
                printf("��");
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
                printf("��");
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
                printf("��");
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
                printf("��");
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
                printf("��");
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
                printf("��");
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
                printf("��");
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
                printf("��");
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
    printf("���ܴ�ǽ������ҧ���Լ�\n");
    Pos(64, 16);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
    Pos(64, 17);
    printf("F1 Ϊ���٣�F2 Ϊ����\n");
    Pos(64, 18);
    printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
    Pos(64, 20);

    // �����������ʾ��ǰ��¼�û���Ϣ�Ĵ���
    if (currentUserIndex!= -1)
    {
        Pos(64, 19);
        printf("***%s ������Ϸ��***", users[currentUserIndex].username);
    }

    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("�÷֣�%d  ", score);
        Pos(64, 11);
        printf("ÿ��ʳ��÷֣�%d ��", add);
    }
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("�÷֣�%d  ", score);
        Pos(64, 11);
        printf("ÿ��ʳ��÷֣�%d ��", add);
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
                    add = 2;//��ֹ���� 1 ֮���ټӻ����д�
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
                    add = 1;  //��֤��ͷ�Ϊ 1
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
    printf("��ӭ����̰ʳ����Ϸ��");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ��� F1 Ϊ���٣�2 Ϊ����\n");
    Pos(25, 13);
    printf("���ٽ��ܵõ����ߵķ�����\n");
    system("pause");
    system("cls");
}

void endgame()
{

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("�Բ�����ײ��ǽ�ˡ���Ϸ����!");
    }
    else if (endgamestatus == 2)
    {
        printf("�Բ�����ҧ���Լ��ˡ���Ϸ����!");
    }
    else if (endgamestatus == 3)
    {
        printf("���Ѿ���������Ϸ��");
    }
    Pos(24, 13);
    printf("���ĵ÷���%d\n", score);
    exit(0);
}
// �� gamestart() �����е���ע�ắ���͵�¼������
// �޸ĺ�� gamestart() ����
void gamestart()
{
    system("mode con cols=100 lines=30");

    while (1)
    {
        int choice;
        printf("��ӭ����̰ʳ����Ϸ��\n");
        printf("1. ע��\n2. ��¼\n3. �˳�\n");
        scanf("%d", &choice);

        if (choice == 1)
        {
            registerUser();
        }
        else if (choice == 2)
        {
            if (loginUser())
            {
                break; // ��¼�ɹ����˳�ѭ������ʼ��Ϸ
            }
        }
        else if (choice == 3)
        {
            exit(0);  // �˳���Ϸ
        }
        system("cls");
    }

    welcometogame(); // ��ӭ��Ϣ
    creatMap();      // ������Ϸ��ͼ
    initsnake();     // ��ʼ����
    createfood();    // ����ʳ��
}

int main()
{
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
