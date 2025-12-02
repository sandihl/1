#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

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
    int ticket_sea;
} Player;

Player player = { 5,0,0,0,0,0,0,0,0,0,0,0,0 };

int shopX = 15;
int caveX = 40;
int homeX = 5;
int farmX = 30;
int trainX = 60;

time_t lastOreTime;
time_t lastHungerTime;
time_t lastFatigueTime;
time_t lastTimeWeatherChange;

HANDLE hConsole;
COORD cursorPos;

enum Weather { SUNNY, RAIN, SNOW };
enum Weather weather = SUNNY;
int daytime = 1;

const char* saveFile = "savegame.dat";

void saveGame() {
    FILE* fp = fopen(saveFile, "wb");
    if (!fp) return;
    fwrite(&player, sizeof(Player), 1, fp);
    fwrite(&daytime, sizeof(int), 1, fp);
    fwrite(&weather, sizeof(enum Weather), 1, fp);
    fclose(fp);
}

void loadGame() {
    FILE* fp = fopen(saveFile, "rb");
    if (!fp) return;
    fread(&player, sizeof(Player), 1, fp);
    fread(&daytime, sizeof(int), 1, fp);
    fread(&weather, sizeof(enum Weather), 1, fp);
    fclose(fp);
}

void gotoxy(int x, int y) {
    cursorPos.X = x;
    cursorPos.Y = y;
    SetConsoleCursorPosition(hConsole, cursorPos);
}

void clearScreen() {
    system("cls");
}

void printStatus() {
    gotoxy(0, 0);
    printf("피로도:%d  배고픔:%d  돈:%d  광석:%d    ",
        player.fatigue, player.hunger, player.money, player.ore);

    gotoxy(0, 1);
    printf("돼지:%d 소:%d 닭:%d | 돼지고기:%d 소고기:%d 닭고기:%d | 티켓:%d     ",
        player.pig, player.cow, player.chicken,
        player.meat_pig, player.meat_cow, player.meat_chicken,
        player.ticket_sea);

    gotoxy(0, 2);
    char* weatherStr = (weather == SUNNY) ? "맑음" : (weather == RAIN) ? "비" : "눈";
    printf("날씨: %s", weatherStr);

    gotoxy(50, 2);
    if (daytime) printf("시간: 낮 ");
    else printf("시간: 밤 ");
}

void printMap() {
    gotoxy(0, 3);
    printf("-----------------------------------------------------------\n");
    printf("\n");
    printf("             **********                   *                *\n");
    printf("             *  SHOP  *                  *                 *\n");
    printf("             *  *  *  *                 *                  *\n");
    printf("             **********               *                    *\n");
    printf("               (상점)                  (동굴)           (기차역)\n");
    printf("\n");
    printf("-----------------------------------------------------------\n");
}

void printHouse() {
    if (!player.home) return;
    gotoxy(homeX - 5, 4);  printf("     *");
    gotoxy(homeX - 5, 5);  printf(" *********");
    gotoxy(homeX - 5, 6);  printf(" *       *");
    gotoxy(homeX - 5, 7);  printf(" *       *");
    gotoxy(homeX - 5, 8);  printf(" *********");
    gotoxy(homeX - 5, 9);  printf("   (집)");
}

void printFarm() {
    if (!player.farm) return;
    gotoxy(farmX - 6, 5);  printf(" ***********");
    gotoxy(farmX - 6, 6);  printf(" *         *");
    gotoxy(farmX - 6, 7);  printf(" *         *");
    gotoxy(farmX - 6, 8);  printf(" ***********");
    gotoxy(farmX - 6, 9);  printf("    (농장)");
}

void printInteractionMessage() {
    if (daytime && player.x >= shopX - 2 && player.x <= shopX + 2) {
        gotoxy(shopX - 5, 15);
        printf("E를 눌러 상점 들어가기");
    }

    if (player.home && player.x >= homeX - 2 && player.x <= homeX + 2) {
        gotoxy(homeX - 5, 15);
        printf("E를 눌러 집 들어가기");
    }

    if (player.farm && player.x >= farmX - 2 && player.x <= farmX + 2) {
        gotoxy(farmX - 5, 15);
        printf("E를 눌러 농장 들어가기");
    }

    if (player.x >= trainX - 2 && player.x <= trainX + 2) {
        gotoxy(trainX - 5, 15);
        if (player.ticket_sea) printf("E를 눌러 기차 탑승");
        else printf("티켓이 필요합니다!");
    }
}

void giveOreIfInCave() {
    time_t now = time(NULL);
    double interval;

    if (weather == SUNNY) interval = 1.0;
    else interval = 1.5; // RAIN, SNOW

    // 밤이면 0.5초 감소
    if (!daytime) interval -= 0.5;
    if (interval < 0.1) interval = 0.1; // 최소 간격 제한

    if (player.x >= caveX - 2 && player.x <= caveX + 2 && now - lastOreTime >= interval) {
        player.ore++;
        lastOreTime = now;
    }
}


void updateOreMessage() {
    if (player.x >= caveX - 2 && player.x <= caveX + 2) {
        gotoxy(0, 17);
        printf("광석을 얻고 있습니다!       ");
    }
    else {
        gotoxy(0, 17);
        printf("                           ");
    }
}

void shopMenu() {
    system("cls");
    int sel;
    while (1) {
        printStatus();
        printf("\n== SHOP ==\n");
        printf("1. 광석3 -> 돈1\n");
        printf("2. 집 구매(10원)\n");
        printf("3. 농장 구매(15원)\n");
        printf("4. 가축 구매(20원)\n");
        printf("5. 고기 굽기\n");
        printf("6. 바다 가는 티켓 구매(100원)\n");
        printf("0. 나가기\n> ");
        scanf("%d", &sel);

        if (sel == 0) break;

        if (sel == 1) {
            if (player.ore >= 3) { player.ore -= 3; player.money++; }
            else { printf("광석이 부족합니다!\n"); Sleep(1000); }
        }
        else if (sel == 2) {
            if (player.home) { printf("이미 집이 있습니다!\n"); Sleep(1000); }
            else if (player.money < 10) { printf("돈이 부족합니다!\n"); Sleep(1000); }
            else { player.money -= 10; player.home = 1; }
        }
        else if (sel == 3) {
            if (player.farm) { printf("이미 농장이 있습니다!\n"); Sleep(1000); }
            else if (player.money < 15) { printf("돈이 부족합니다!\n"); Sleep(1000); }
            else { player.money -= 15; player.farm = 1; }
        }
        else if (sel == 4) {
            if (!player.farm) {
                printf("농장을 먼저 구매해야 합니다!\n");
                Sleep(1000);
                system("cls");
                continue;
            }
            if (player.money < 20) {
                printf("돈이 부족합니다!\n");
                Sleep(1000);
                system("cls");
                continue;
            }

            int c;
            printf("1. 돼지 2. 소 3. 닭\n> ");
            scanf("%d", &c);

            if (c == 1) player.pig++;
            else if (c == 2) player.cow++;
            else if (c == 3) player.chicken++;

            player.money -= 20;
        }
        else if (sel == 5) {
            int c; printf("1. 돼지고기 2. 소고기 3. 닭고기\n> "); scanf("%d", &c);

            if (player.hunger == 0) {
                printf("이미 배고픔이 0입니다!\n");
                Sleep(1000);
                system("cls");
                continue;
            }

            if (c == 1 && player.meat_pig > 0) { player.meat_pig--; player.hunger -= 10; if (player.hunger < 0)player.hunger = 0; }
            else if (c == 2 && player.meat_cow > 0) { player.meat_cow--; player.hunger -= 10; if (player.hunger < 0)player.hunger = 0; }
            else if (c == 3 && player.meat_chicken > 0) { player.meat_chicken--; player.hunger -= 10; if (player.hunger < 0)player.hunger = 0; }
            else { printf("고기 없음!\n"); Sleep(1000); }
        }
        system("cls");
    }
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
        system("cls");
    }
}

void farmMenu() {
    system("cls");
    int sel;
    while (1) {
        printStatus();
        printf("\n== 농장 ==\n1. 돼지고기 얻기\n2. 소고기 얻기\n3. 닭고기 얻기\n0. 나가기\n> ");
        scanf("%d", &sel);

        if (sel == 1) {
            if (player.pig > 0) { player.pig--; player.meat_pig++; }
            else { printf("돼지가 없습니다!\n"); Sleep(1000); }
        }
        else if (sel == 2) {
            if (player.cow > 0) { player.cow--; player.meat_cow++; }
            else { printf("소가 없습니다!\n"); Sleep(1000); }
        }
        else if (sel == 3) {
            if (player.chicken > 0) { player.chicken--; player.meat_chicken++; }
            else { printf("닭이 없습니다!\n"); Sleep(1000); }
        }
        else if (sel == 0) break;

        system("cls");
    }
}

void printSeaScreen() {
    system("cls");
    printf("-----------------------------------------------------------\n\n");
    printf("             ~~~~~~~~****~~~~~~~***~~~~~~~****~~~~~~~\n");
    printf("             ***~~~~~~~****~~~~~~~***~~~~~~~****~~~~~\n");
    printf("             ~~~~~****~~~~~~~****~~~~~~~****~~~~~~~**\n");
    printf("             ***~~~~~~~****~~~~~~~***~~~~~~~****~~~~~\n");
    printf("                           ( 바   다 )\n");
    printf("\n-----------------------------------------------------------\n");
}

int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    lastOreTime = time(NULL);
    lastHungerTime = time(NULL);
    lastFatigueTime = time(NULL);
    lastTimeWeatherChange = time(NULL);

    srand(time(NULL));
    weather = rand() % 3;

    loadGame();

    clearScreen();
    int redraw = 1;

    while (1) {
        time_t now = time(NULL);

        if (now - lastTimeWeatherChange >= 60) {
            daytime = !daytime;
            weather = rand() % 3;
            lastTimeWeatherChange = now;
            redraw = 1;
        }

        // 배고픔 시간 계산
        double hungerInterval = 5; // 맑음
        if (weather == RAIN) hungerInterval = 4;
        else if (weather == SNOW) hungerInterval = 4; // 눈은 4초 +1
        if (now - lastHungerTime >= hungerInterval) {
            player.hunger++;
            lastHungerTime = now;
            redraw = 1;
        }

        // 피로도 시간 계산
        double fatigueInterval = 3; // 맑음
        if (weather == RAIN) fatigueInterval = 2;
        else if (weather == SNOW) fatigueInterval = 2; // 눈은 2초 +1
        if (now - lastFatigueTime >= fatigueInterval) {
            player.fatigue++;
            lastFatigueTime = now;
            redraw = 1;
        }

        giveOreIfInCave();

        if (player.hunger >= 100 || player.fatigue >= 100) {
            clearScreen();
            gotoxy(10, 10);
            printf("===== GAME OVER =====");
            Sleep(3000);
            return 0;
        }

        if (redraw) {
            clearScreen();
            printStatus();
            printMap();
            printHouse();
            printFarm();
            printInteractionMessage();
            gotoxy(player.x, 10); printf("*");
            redraw = 0;
        }

        updateOreMessage();

        if (kbhit()) {
            int key = getch();
            redraw = 1;

            if (key == 224) {
                key = getch();
                if (key == 75 && player.x > 0) player.x--;
                if (key == 77 && player.x < trainX) player.x++;
            }
            else if (key == 'e' || key == 'E') {
                if (player.x >= shopX - 2 && player.x <= shopX + 2 && daytime)
                    shopMenu();
                else if (player.home && player.x >= homeX - 2 && player.x <= homeX + 2)
                    homeMenu();
                else if (player.farm && player.x >= farmX - 2 && player.x <= farmX + 2)
                    farmMenu();
                else if (player.x >= trainX - 2 && player.x <= trainX + 2 && player.ticket_sea) {
                    player.ticket_sea = 0;
                    printSeaScreen();
                    Sleep(2000);
                    gotoxy(0, 15);
                    printf("바다에 도착했습니다!");
                    Sleep(2000);
                    gotoxy(0, 16);
                    printf("===== GAME CLEAR =====");
                    saveGame();
                    Sleep(2000);
                    return 0;
                }
            }
            else if (key == 'q' || key == 'Q') {
                saveGame();
                clearScreen();
                gotoxy(0, 10);
                printf("게임 저장 후 종료합니다!\n");
                Sleep(2000);
                return 0;
            }
            else if (key == 'r' || key == 'R') {
                player = (Player){ 5,0,0,0,0,0,0,0,0,0,0,0,0 };
                daytime = 1;
                weather = SUNNY;
                lastOreTime = lastHungerTime = lastFatigueTime = lastTimeWeatherChange = time(NULL);
                remove(saveFile);
                clearScreen();
                gotoxy(0, 0);
                printf("게임이 초기화되었습니다!\n");
                Sleep(1000);
                redraw = 1;
            }
        }

        Sleep(10);
    }

    return 0;
}
