#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

typedef struct {
    int x;
    int money;
    int hunger;
    int fatigue;
    int ore;
    int home;
    int farm;
    int pig, cow, chicken;
    int meat_pig, meat_cow, meat_chicken;
} Player;

Player player = { 5,0,0,0,0,0,0,0,0,0,0,0 };

int shopX = 12;
int caveX = 35;
int homeX = 20;
int farmX = 28;

time_t lastOreTime;

HANDLE hConsole;
COORD cursorPos;

void gotoxy(int x, int y) {
    cursorPos.X = x;
    cursorPos.Y = y;
    SetConsoleCursorPosition(hConsole, cursorPos);
}

void printStatus() {
    gotoxy(0, 0);
    printf("피로도:%d  배고픔:%d  돈:%d  광석:%d    ",
        player.fatigue, player.hunger, player.money, player.ore);

    gotoxy(0, 1);
    printf("돼지:%d 소:%d 닭:%d | 돼지고기:%d 소고기:%d 닭고기:%d      ",
        player.pig, player.cow, player.chicken,
        player.meat_pig, player.meat_cow, player.meat_chicken);
}

void printMap() {
    gotoxy(0, 3);
    printf("------------------------------------------------\n");
    printf("\n");
    printf("            ***********          *\n");
    printf("            *  SHOP  *          *\n");
    printf("            *  *  *  *          *\n");
    printf("            ***********        *\n");
    printf("               (상점)      (동굴)\n");
    printf("\n");
    printf("------------------------------------------------\n");
}

void giveOreIfInCave() {
    if (player.x == caveX) {
        time_t now = time(NULL);
        if (now - lastOreTime >= 5) {
            player.ore++;
            lastOreTime = now;
        }
    }
}

// --------------------------
// 메뉴 함수들
// --------------------------
void shopMenu() {
    system("cls");
    int sel;

    while (1) {
        printStatus();
        printf("\n== SHOP ==\n1. 광석3 -> 돈1\n2. 집 구매(10원)\n3. 농장 구매(15원)\n4. 가축 구매(40원)\n5. 고기 굽기\n0. 나가기\n> ");
        scanf("%d", &sel);

        if (sel == 0) break;

        if (sel == 1 && player.ore >= 3) { player.ore -= 3; player.money++; }
        else if (sel == 2 && player.money >= 10) { player.money -= 10; player.home = 1; }
        else if (sel == 3 && player.money >= 15) { player.money -= 15; player.farm = 1; }
        else if (sel == 4 && player.money >= 40) {
            int c;
            printf("1. 돼지 2. 소 3. 닭\n> ");
            scanf("%d", &c);
            if (c == 1) player.pig++;
            else if (c == 2) player.cow++;
            else if (c == 3) player.chicken++;
            player.money -= 40;
        }
        else if (sel == 5) {
            int c;
            printf("1. 돼지고기 2. 소고기 3. 닭고기\n> ");
            scanf("%d", &c);
            if (c == 1 && player.meat_pig > 0) { player.meat_pig--; player.hunger -= 10; }
            else if (c == 2 && player.meat_cow > 0) { player.meat_cow--; player.hunger -= 10; }
            else if (c == 3 && player.meat_chicken > 0) { player.meat_chicken--; player.hunger -= 10; }
            else { printf("고기 없음!\n"); Sleep(1000); }
        }
    }

    system("cls");
}

void homeMenu() {
    system("cls");

    int sel;
    while (1) {
        printStatus();
        printf("\n== 집 ==\n1. 잠자기 (피로도 0)\n0. 나가기\n> ");
        scanf("%d", &sel);

        if (sel == 1) { player.fatigue = 0; break; }
        if (sel == 0) break;
    }

    system("cls");
}

void farmMenu() {
    system("cls");

    int sel;
    while (1) {
        printStatus();
        printf("\n== 농장 ==\n1. 돼지고기 얻기\n2. 소고기 얻기\n3. 닭고기 얻기\n0. 나가기\n> ");
        scanf("%d", &sel);

        if (sel == 1) player.meat_pig++;
        else if (sel == 2) player.meat_cow++;
        else if (sel == 3) player.meat_chicken++;
        else if (sel == 0) break;
    }

    system("cls");
}

// --------------------------
// 메인
// --------------------------
int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    lastOreTime = time(NULL);

    system("cls");

    while (1) {
        printStatus();
        printMap();
        giveOreIfInCave();

        gotoxy(player.x, 10);
        printf("*");

        // 모든 키 여기서만 처리
        if (kbhit()) {
            int key = getch();

            // 방향키
            if (key == 224) {
                key = getch();
                if (key == 75 && player.x > 0) player.x--;      // 왼쪽
                if (key == 77 && player.x < 50) player.x++;     // 오른쪽
            }
            // E키 상호작용
            else if (key == 'e' || key == 'E') {
                if (player.x == shopX)       shopMenu();
                else if (player.home && player.x == homeX)  homeMenu();
                else if (player.farm && player.x == farmX)  farmMenu();
            }
        }

        Sleep(50);
        system("cls");
    }

    return 0;
}
