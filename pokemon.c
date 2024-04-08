#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<windows.h>
/// 4/8 10:49 최종 수정 시간
//
typedef struct PK {
    char name[20];
    char type[10];
    char nickname[20];
    int hp;
    int pre_hp;
    int st;
    int num;
}PK;

typedef struct wallet {
    int money;
    int monsterball;
    int medicine;
}wallet;

wallet wt = {10000,5,5}; // 지갑
int pk_num = 0; // 현재 보유 포켓몬 수

void input_member(PK* pk, char* buffer, int member);
void init_pokemon(PK* pk);
void pokemon_book(PK* pk);
void my_pokemon(PK* pk, int len);
void select_first_pokemon(PK* pk);
int getTotalLine(char* txt);
void main_game(PK* mine);
void adventure(PK* mine, PK enemy, int idx);
void select_next(PK* mine, PK enemy, int idx, int turn);
int run(int hp, int present_hp);
void my_bag(PK* mine, PK enemy, int idx);
void attack(PK* mine, PK enemy, int idx, int turn);
void fight_1st(PK* mine, PK* enemy, int idx);
void fight_2nd(PK* mine, PK* enemy, int idx);
int priority(char* type_em, char* type_my);
void pokemon_center(PK* mine);
void store(PK* mine);
void pokemon_book(PK* mine);
void backup(PK* mine);
int AreYouAlive(PK* pk, int idx); 
int CallSavefile(PK* mine);
void Checking_HP(PK* mine);
void my_turn(PK* mine, PK* enemy, int idx, int more_strong, int my_st,int em_st);
void wild_turn(PK* mine, PK* enemy, int idx, int more_strong, int my_st,int em_st);
void enter();

int main() {  
    srand((unsigned int)time(NULL)); system("chcp 65001"); // gcc 한글 깨짐
    int first_pk, how_many, idx = 1;
    int new_game, repet=0; char input[5];
    PK* mine;
    
    printf("\t========================================\n");
    printf("\t\tK.Knock Pokemon Game\n\n\t\tpress enter to start\n");
    printf("\t========================================\n");
    
    while(1){gets(input); if(input[0]==0) break;}  // 엔터 입력 전까지 무한 반복
    char txt[30] = "savefile.txt";
    how_many = getTotalLine(txt) - 2; // savefile에 라인수 출력 savefile은 \n 포함해서 라인수 셈
    if(how_many < 0) {
        mine = (PK*)malloc(sizeof(PK)); // 첫 포켓몬 1마리 받을 만큼만 크기 설정
        select_first_pokemon(&mine[0]); // 인덱스 0번째에 넣어주기
    }
    else if(how_many > 0){ // 저장된 파일 존재 
        do {
            if(repet>0) printf("잘못된 선택입니다.\n");
            if(repet>4) {printf("프로그램을 강제로 종료합니다..\n"); exit(0);}
            printf("\t1. 새로하기  2. 이어하기\n >>");
            scanf("%d", &new_game);
            printf("\n\n");
            repet++;
        } while (!(new_game==1||new_game==2));    

        if (new_game == 1) {
            mine = (PK*)malloc(sizeof(PK)); // 첫 포켓몬 1마리 받을 만큼만 크기 설정
            select_first_pokemon(&mine[0]); // 인덱스 0번째에 넣어주기
        }
        else if (new_game == 2) {
            FILE* fp = fopen("savefile.txt", "r"); // 파일 비었는지 확인
            if (fp != NULL) { // 파일 존재
                mine = (PK*)malloc(how_many * sizeof(PK));  // 내 포켓몬
                idx = CallSavefile(mine);
            }
            fclose(fp);
        } 
    }
    pk_num = idx; // 전역변수 포켓몬 수에 count한거 넣어주기
    main_game(mine); // 메인게임화면으로 내 포켓몬 들고 이동 
    free(mine);
}
void main_game(PK* mine) {
    int chk,repet=0;
    int pk_idx;

    do {
        if(repet>0) printf("잘못된 선택입니다.\n");
        if(repet>4) {printf("프로그램을 강제로 종료합니다..\n"); exit(0);}
        printf("\n\n\t========================================\n");
        printf("\t모험을 진행하시겠습니까?\n");
        printf("\t1. 네 2. 저장 3. 상점 4. 포켓몬센터 5. 포켓몬 도감\n >>");
        scanf("%d", &chk); repet++;
    } while (!(chk == 1 || chk == 2 || chk == 3 || chk == 4 || chk == 5));
    
    int wprn = rand() % 10; //wild_pokemon_random_num 0~9 
    repet=0;
    if (chk == 1) {
        Checking_HP(mine); // 살아있는 포켓몬 있는지 확인 없으면 센터로 ㄱㄱ

        do {
            if(repet>0){printf("잘못된 선택입니다.\n");}
            if(repet>5) {printf("반복된 범위 외 입력으로 프로그램을 종료시킵니다..\n"); exit(0);}
            printf("어떤 포켓몬을 선택하겠습니까?\n");
            my_pokemon(mine, pk_num);
            printf("\n>> ");
            scanf("%d", &pk_idx); repet++;
        } while ((pk_idx <= 0 || pk_idx > pk_num));

        while (!AreYouAlive(mine, pk_idx - 1)) { // 현재 체력 0이하 선택 시 다시 선택하라고 하기
            printf("현재 선택한 포켓몬은 많이 아픕니다.. \n어떤 포켓몬을 선택하겠습니까?\n");
            my_pokemon(mine, pk_num);
            printf("\n>> ");
            scanf("%d", &pk_idx);
        }
        char txt[30] = "pokemon.txt";
        int wild_num = getTotalLine(txt) + 1;// pokemon.txt 줄수 +1 \n 포함이면 오류남 
        PK* wild = (PK*)malloc(wild_num * sizeof(PK)); init_pokemon(wild); // 포켓몬 10마리 초기화
        PK enemy = wild [wprn];
        enemy.hp += rand()%151; enemy.pre_hp = enemy.hp; enemy.st += rand()%101;
        free(wild); // 해제함
        adventure(mine, enemy, pk_idx - 1);
    }
    else if (chk == 2) {backup(mine); exit(0);}
    else if (chk == 3) store(mine);
    else if (chk == 4) {pokemon_center(mine);}
    else if (chk == 5) {pokemon_book(mine);}
}
void adventure(PK* mine, PK enemy, int idx) {
    int delay = 1000 + rand() % 4001;  // 1~5초 
    int turn;
    system("cls"); //콘솔화면 너무 길어지니 이전 거 지워놓기
    printf("\t포켓몬을 탐색하는 중 ...\n");
    Sleep(delay); // 딜레이

    printf("\t========================================\n");
    printf("앗! 야생의 포켓몬이 나타났다!\n\n");
    printf("\t\t\t\t야생 포켓몬: %s\n", enemy.name);
    printf("\t\t\t\t현재HP: %d /Max HP: %d \n\n", enemy.pre_hp, enemy.hp);
    printf("내 포켓몬: %s(%s)\n", mine[idx].name, mine[idx].nickname);
    printf("현재HP: %d /Max HP: %d \n\n", mine[idx].pre_hp, mine[idx].hp);

    turn = rand() % 2; // 0~1  상대 || 나 선공 고르기
    select_next(mine, enemy, idx, turn);
}
void select_next(PK* mine, PK enemy, int idx, int turn) {
    int select_ad, repet=0;

    do {
        if(repet>0) printf("잘못된 선택입니다.\n");
        if(repet>4) {printf("프로그램을 강제로 종료합니다..\n"); exit(0);}
        printf("\t========================================\n");
        printf("\t무엇을 해야할까?\n");
        printf("\t1. 공격 2. 가방열기 3. 도망치기\n >> ");
        scanf("%d", &select_ad); repet++;
   }while(!(select_ad == 1 || select_ad == 2 || select_ad == 3)); 

    if (select_ad == 1) attack(mine, enemy, idx, turn);
    else if (select_ad == 2) my_bag(mine, enemy, idx);
    else if (select_ad == 3) { 
        int esape = run(enemy.hp, enemy.pre_hp);
        if (esape) { printf("성공적으로 도망쳤다!\n"); main_game(mine);}
        else { printf("도망에 실패했다..!\n"); select_next(mine, enemy, idx, turn); }
    }
}
void attack(PK* mine, PK enemy, int idx, int turn) {
    if (turn == 0)  fight_1st(mine, &enemy, idx);  // 내가 먼저
    else fight_2nd(mine, &enemy, idx);             // 상대 먼저

    printf("\t========================================\n");
    printf("\t\t\t\t야생 포켓몬: %s\n", enemy.name);
    printf("\t\t\t\t현재HP: %d /Max HP: %d \n\n", enemy.pre_hp, enemy.hp);
    printf("내 포켓몬: %s(%s)\n", mine[idx].name, mine[idx].nickname);
    printf("현재HP: %d /Max HP: %d \n\n", mine[idx].pre_hp, mine[idx].hp);
    select_next(mine, enemy, idx, turn); // 다시 select_next 으로 돌아가 뭐할지 선택        
}
void fight_1st(PK* mine, PK* enemy, int idx) { // 내가 먼저
    int my_st = mine->st, em_st = enemy->st;
    int more_strong = priority(enemy->type, mine[idx].type); // 타입 별 우선순위 정함 +:상대가 위 -:내가위 0동급
    // int형으로 형변환 전 0.5를 더해 반올림
    if (more_strong > 0) {em_st = (int)((double)em_st * (1.5) + 0.5); my_st = (int)((double)my_st * (0.5) + 0.5);} 
    else if (more_strong < 0) {my_st = (int)((double)my_st * (1.5) + 0.5); em_st = (int)((double)em_st * (0.5) + 0.5);}
    my_turn(mine,enemy,idx,more_strong,my_st,em_st);   
    wild_turn(mine,enemy,idx,more_strong,my_st,em_st);
}
void fight_2nd(PK* mine, PK* enemy, int idx) {
    int my_st = mine->st, em_st = enemy->st;
    int more_strong = priority(enemy->type, mine[idx].type);

    if (more_strong > 0) {em_st = (int)((double)em_st * (1.5) + 0.5); my_st = (int)((double)my_st * (0.5) + 0.5);} 
    else if (more_strong < 0) {my_st = (int)((double)my_st * (1.5) + 0.5); em_st = (int)((double)em_st * (0.5) + 0.5);}
    wild_turn(mine,enemy,idx,more_strong,my_st,em_st); 
    my_turn(mine,enemy,idx,more_strong,my_st,em_st);   
}
void my_bag(PK* mine, PK enemy, int idx) {
    int select,repet = 0;

    do {
        if(repet>0){printf("잘못된 선택입니다.\n");}
        if(repet>5) {printf("반복된 범위 외 입력으로 프로그램을 종료시킵니다..\n"); exit(0);}
        printf("\t========================================\n");
        printf("\t1. 몬스터볼 X %d\n", wt.monsterball);
        printf("\t2. 회복약 X %d\n >> ", wt.medicine);
        scanf("%d", &select); repet++;
    } while (!(select == 1 || select == 2));

    if ((select == 2 && wt.medicine == 0) || (select == 1 && wt.monsterball == 0)) {
        printf("아이템이 부족합니다.. \n"); select_next(mine, enemy, idx, 1); // 상대 먼저
    }
    else if (select == 2 && wt.medicine > 0) {
        wt.medicine--;
        int heel = (int)(mine[idx].hp * 0.3 + 0.5);
        int max_hp = mine[idx].hp, pre_hp = mine[idx].pre_hp;

        if (max_hp - pre_hp > heel) mine[idx].pre_hp += heel; // 전체HP의 30% 힐 
        else mine[idx].pre_hp = max_hp; // 피해량 30%보다 적으면 풀피로
        printf("상처약을 %s(%s)에게 주었다.\n", mine[idx].name, mine[idx].nickname);
        printf("--%s(%s)의 HP : %d\n\n", mine[idx].name, mine[idx].nickname, mine[idx].pre_hp);
        attack(mine, enemy, idx, 1); // 상대먼저 바로 시작
    }
    else if (select == 1 && wt.monsterball > 0) {
        wt.monsterball--;
        int randon_num = rand() % 100 + 1;
        double hp_percent = (double)(enemy.pre_hp) / (double)enemy.hp * 100;
        int percent = 100 - (int)(hp_percent + 0.5);

        if (randon_num <= percent) {
            realloc((PK*)mine, sizeof(PK) * (pk_num + 1)); // mine 메모리 재할당
            int chk; char nick[20];
            printf("%s을 잡았다! \n", enemy.name);
            printf("별명을 정하기 1(yes)/0(no)\n >> ");
            scanf("%d", &chk);
            if (chk == 1) {
                printf("별명 >> ");
                scanf("%s", nick);
            }
            else {printf("no\n");strcpy(nick, enemy.name);}
            
            strcpy(mine[pk_num].name, enemy.name);
            strcpy(mine[pk_num].nickname, nick);
            strcpy(mine[pk_num].type, enemy.type);
            mine[pk_num].hp = enemy.hp;
            mine[pk_num].pre_hp = enemy.pre_hp;
            mine[pk_num].st = enemy.st;

            pk_num++; // 내 포켓몬 수 증가
            
            if (pk_num > 5) { // 내 포켓몬 6마리 이상이면 포켓몬 마스터
                int re;
                printf("포켓몬 마스터가 되었다!\n\n");
                my_pokemon(mine,6);
                printf("\n 돈: %d\n 포켓몬볼 x %d\n 상처약 x %d\n",wt.money,wt.monsterball,wt.medicine);
                printf("게임을 재시작 하겠습니까? \n 1(yes) / 0(no) >> ");
                scanf("%d", &re);
                if (re == 1) { 
                    printf("재시작\n"); 
                    free(mine); system("cls");
                    pk_num = 1; wt.money = 10000; wt.medicine = wt.monsterball = 5;
                    main();
                }
                else { printf("TEH END...\n"); free(mine); exit(0); }
            }
            main_game(mine);
        }
        else { 
            printf("%s이 몬스터볼에서 빠져나왔다..! \n", enemy.name);
            attack(mine, enemy, idx, 1);  // 상대먼저 때려
        }
    }
}
int run(int hp, int present_hp) { 
    int per = rand() % 100 + 1; //1~100
    double p = (double)present_hp / (double)hp *100;

    if(p == 100) return(per<=10)?1:0; // 10%
    else if(p >= 60) return (per<=40)?1:0; // 40%
    else if(p >= 40) return (per<=60)?1:0; // 40%
    else  return (p<=90)?1:0; // 90%
}
void backup(PK* mine) {
    FILE* fp = fopen("savefile.txt", "w");

    for (int i = 0; i < pk_num; i++) {
        fprintf(fp, "%s %s %s %d %d %d\n", mine[i].name, mine[i].nickname, mine[i].type, mine[i].hp, mine[i].pre_hp, mine[i].st);
    }
    fprintf(fp, "next\n%d %d %d\n", wt.money, wt.monsterball, wt.medicine);
    printf("저장되었습니다!\n");
    fclose(fp); free(mine); 
}
void init_pokemon(PK* pk) { // 파일에 있는 포켓몬을 포켓몬 구조체 배열로 만들어서 보관해놈
    FILE* fp = fopen("pokemon.txt", "r");
    if (fp != NULL) {
        char buffer[30];
        int idx_w = 0; 
        while (!feof(fp)) { // 파일의 끝에 도달할 때까지   
            int member = 1;  // 멤버변수 ex 1:num 2:name ...
            fgets(buffer, sizeof(buffer), fp); // 버퍼에 한라인씩 입력받음
            char* ptr = strtok(buffer, " "); // 한 줄 읽어옴
            while (ptr != NULL) { // 줄 공백 단위로 문자열 저장 
                input_member(&(pk[idx_w]), ptr, member++);
                ptr = strtok(NULL, " ");
            }idx_w++; // 파일에 있는 포켓몬 수 count
        }
        fclose(fp);
    }
}
void input_member(PK* pk, char* buffer, int member) { // 포켓몬 맴버 변수들에 값 넣어주기
    if(member == 1) pk->num = atoi(buffer);
    else if(member == 2) strcpy(pk->name, buffer);
    else if(member == 3) strcpy(pk->type, buffer);
    else if(member == 4) pk->st = atoi(buffer);
    else if(member == 5) pk->hp = atoi(buffer);
    pk->pre_hp = pk->hp;
}
void select_first_pokemon(PK* pk) {
    int start_pokemon, repet=0;
    char nickname[20];

    do {
        if(repet>0) printf("잘못된 선택입니다.\n");
        if(repet>4) {printf("반복된 범위 외 입력으로 프로그램을 종료시킵니다..\n"); exit(0);}
        printf("\t========================================\n");
        printf("\t어느 포켓몬을 선택하시겠습니까?\n\t\t1. 파이리 2. 이상해씨 3. 꼬부기\n >> ");
        scanf("%d", &start_pokemon); repet++;
   }while(!(start_pokemon == 1 ||start_pokemon == 2 || start_pokemon == 3)); 

    printf("\n\n별명: ");
    scanf("%s", nickname);
    strcpy(pk->nickname, nickname);
    pk->st = 80 + rand() % 101; pk->hp = 300 + rand() % 151; pk->pre_hp = pk->hp;

    if (start_pokemon == 1) { strcpy(pk->name, "파이리"); strcpy(pk->type, "불"); pk->num = 2; }
    else if (start_pokemon == 2) { strcpy(pk->name, "이상해씨"); strcpy(pk->type, "풀"); pk->num = 1; }
    else if (start_pokemon == 3) { strcpy(pk->name, "꼬부기"); strcpy(pk->type, "물"); pk->num = 3; }
}

void my_pokemon(PK* pk, int len) {
    printf("포켓몬 도감\n");
    for (int i = 0; i < len; i++) 
    printf("%2d.  %s(%s) %s %d/%d - %d\n", i + 1, pk[i].name, pk[i].nickname, pk[i].type, pk[i].hp, pk[i].pre_hp, pk[i].st);
}
int priority(char* type_em, char* type_my) {
    int em = 2, my = 0;
    if (!strcmp(type_em, "풀")) (!strcmp(type_my, "풀"))?my=2:(!strcmp(type_my, "물")?my=1:!strcmp(type_my, "불")?my=3:-1);  
    else if (!strcmp(type_em, "물")) (!strcmp(type_my, "풀"))?my=3:(!strcmp(type_my, "물")?my=2:!strcmp(type_my, "불")?my=1:-1);
    else if (!strcmp(type_em, "불")) (!strcmp(type_my, "풀"))?my=1:(!strcmp(type_my, "물")?my=3:!strcmp(type_my, "불")?my=2:-1);
    return (em > my) ? 1 : ((em == my) ? 0 : -1);
}
void store(PK* mine) {
    int select, num, repet=0;
    
    printf("\t========================================\n");
    printf("\t 상점\t\t\t지갑 : %d원\n",wt.money);

    do {
        if(repet>0)  printf("잘못된 선택입니다.\n"); 
        if(repet>5) { printf("반복된 범위 외 입력으로 프로그램을 종료시킵니다..\n"); exit(0);}
        printf("\t========================================\n");
        printf("\t1. 포켓몬볼  1000원\n\t2. 회복약 500원\n\n");    
        printf("무엇을 구매할까?(나가기 0)\n >> ");
        scanf("%d", &select); repet++;
    } while (!(select == 1 || select == 2 ||select == 0));    

    if (select == 0) main_game(mine);
    else if (select == 1) {
        printf("포켓몬볼을 몇개 구매할까?(취소 -1)\n >> ");
        scanf("%d", &num);
        if (num == -1) { store(mine); }
        else if(num > 0){
            if (1000 * num > wt.money) printf("돈이 부족해요.. \n");
            else {
                wt.monsterball += num; wt.money -= 1000 * num;
                printf("성공적으로 구매했다! (잔액 : %d원)\n", wt.money);
            }
        }
        else if(num < 0){ printf("음수를 입력했습니다. 양수를 입력해야합니다,\n"); store(mine); }
    }
    else if (select == 2) {
        printf("회복약을 몇개 구매할까?(취소 -1)\n >> ");
        scanf("%d", &num);
        if (num == -1)  store(mine); 
        else if(num > 0){
            if (500 * num > wt.money) printf("돈이 부족해요.. \n");
            else {
                wt.medicine += num; wt.money -= 500 * num;
                printf("성공적으로 구매했다! (잔액 : %d원)\n", wt.money);
            }
        }
        else if(num < 0){ printf("음수를 입력했습니다. 양수를 입력해야합니다,\n"); store(mine); }
    }
    main_game(mine);
}
void pokemon_center(PK* mine) { 
    printf("\t========================================\n");
    printf("\t포켓몬 회복 중 ...\n\n");
    Sleep(2000+rand()%1001); // after 2s~3s
    for (int i = 0; i < pk_num + 1; i++) mine[i].pre_hp = mine[i].hp;
    printf("\t회복이 완료되었습니다!\n");
    my_pokemon(mine,pk_num);

    printf("(엔터 입력 시 메인으로..)\n");
    enter();
    
    main_game(mine);
}
void pokemon_book(PK* mine) {
    char txt[30] = "pokemon.txt";
    int wild_num = getTotalLine(txt) + 1;
    PK* pk = (PK*)malloc(sizeof(PK) * wild_num); // 해제함
    init_pokemon(pk);
    printf("포켓몬 도감\n");
    for (int i = 0; i < 10; i++) 
        printf("%2d. %s(%s) %d(HP) %d(공격력)\n", pk[i].num, pk[i].name, pk[i].type, pk[i].hp, pk[i].st);

    printf("(엔터 입력 시 메인으로..)\n");
    enter();
    
    free(pk);
    main_game(mine);
}
int AreYouAlive(PK* pk, int idx) {return (pk[idx].pre_hp <= 0) ? 0 : 1;}
int getTotalLine(char *txt) {
    FILE* fp;
    int line = 0; char c;
    fp = fopen(txt, "r");
    while ((c = fgetc(fp)) != EOF)
        if (c == '\n') line++;
    fclose(fp); return line;
}
int CallSavefile(PK* mine){
    const int MaxSize = 100; 
    char* line = (char*)malloc(MaxSize* sizeof(char)); // 해제
    char* pLine;
    int idx=0;
    FILE* in = fopen("savefile.txt", "r");

    while (!feof(in)) { 
        pLine = fgets(line, MaxSize, in); // 한 줄씩 받아냄
        if (pLine == NULL) break;

        else if (!strcmp(pLine, "next\n")) { // next 다음 부터는 지갑관련 내용 받아내기
            pLine = fgets(line, MaxSize, in);
            char* ptr = strtok(pLine, " "); // 공백 단위로 문자열 끊기
            int nn = 0; // 돈 포켓몬볼 상처약 순으로
            while (ptr != NULL) {
                if (nn == 0) { wt.money = atoi(ptr); }
                else if (nn == 1) wt.monsterball = atoi(ptr);
                else if (nn == 2) wt.medicine = atoi(ptr);
                ptr = strtok(NULL, " ");
                nn++;
            }
        }
        else {
            int member = 0, number;
            char* ptr = strtok(pLine, " ");
            while (ptr != NULL) {
                if (member == 0) strcpy(mine[idx].name, ptr);
                else if (member == 1) strcpy(mine[idx].nickname, ptr);
                else if (member == 2) strcpy(mine[idx].type, ptr);
                else if (member == 3) { number = atoi(ptr); mine[idx].hp = number; }
                else if (member == 4) { number = atoi(ptr); mine[idx].pre_hp = number; }
                else if (member == 5) { number = atoi(ptr); mine[idx].st = number; }
                ptr = strtok(NULL, " ");
                member++;
            }
        }
        idx++; // 포켓몬 몇마리인지 count
    }
    fclose(in); free(line); // line 동적메모리 해제
    printf("money: %d monsterball:%d medicine:%d\n", wt.money, wt.monsterball, wt.medicine);
    return idx-1;
}
void Checking_HP(PK* mine){
    int live=0; // 살아있는 내 포켓몬 있나 확인 없으면 포켓몬 센터로 가기
    for(int i=0;i<pk_num;i++){
        if(AreYouAlive(mine,i)) {live ++; return;}
    }
    if(live == 0) {
        printf("모든 포켓몬들의 현재 HP가 0이하입니다. \n포켓몬 센터로 이동합니다...\n");
        pokemon_center(mine);
    }
}
void my_turn(PK* mine, PK* enemy, int idx, int more_strong, int my_st,int em_st){
    int cri, critical = rand() % 10 + 1; //1~10 크리확률 구하기
    printf("\t========================================\n");
    printf("\t%s(%s)의 공격!\n", mine[idx].name, mine[idx].nickname);
    if (critical <= 2) { // 1 || 2면 크리터진걸로 check
        cri = (int)((double)my_st * (1.5) + 0.5);
        printf("\t%s(은)는 %d의 데미지를 입었다.\n", enemy->name, cri); printf("\t급소에 맞았다.\n");
        enemy->pre_hp -= cri;
    }
    else {
        printf("\t%s(은)는 %d의 데미지를 입었다.\n", enemy->name, my_st);
        if (more_strong < 0) printf("\t효과가 굉장했다.\n");
        else if (more_strong > 0) printf("\t효과가 별로인 듯 하다.\n");
        enemy->pre_hp -= my_st;
    }
    if (enemy->pre_hp <= 0) {
        printf("\n\n야생 포켓몬이 쓰러졌다!\n"); int coin= 300+ rand()%201; wt.money+=coin;  
        printf("\t==================%d원을 획득했다.====\n현재 money: %d 원==================\n",coin,wt.money);
        main_game(mine);
    }
}
void wild_turn(PK* mine, PK* enemy, int idx, int more_strong, int my_st,int em_st){
    int cri, critical = rand() % 10 + 1;
    printf("\t========================================\n");
    printf("\t\t\t\t%s의 공격!\n", enemy->name);
    if (critical <= 2) {
        cri = em_st + (int)((double)em_st * (1.5) + 0.5);
        printf("\t%s(%s)(은)는 %d의 데미지를 입었다.\n", mine[idx].name, mine[idx].nickname, cri); printf("\t급소에 맞았다.\n");
        mine[idx].pre_hp -= cri;
    }
    else {
        printf("\t%s(%s)(은)는 %d의 데미지를 입었다.\n", mine[idx].name, mine[idx].nickname, em_st);
        if (more_strong > 0) printf("\t효과가 굉장했다.\n");
        else if (more_strong < 0) printf("\t효과가 별로인 듯 하다.\n");
        mine[idx].pre_hp -= em_st;
    }
    if (mine[idx].pre_hp <= 0) {
        int num, cnt = 0;
        printf("\t========================================\n");
        printf("%s(%s)가 기절했다..\n", mine[idx].name, mine[idx].nickname);
        printf("\n\n나의 포켓몬 도감\n");
        int deadPk[7] = {0,};
        for (int i = 0; i < pk_num; i++) {
            if (mine[i].pre_hp > 0) { 
                printf("%2d.  %s(%s) %s %d/%d - %d\n", i + 1, mine[i].name, mine[i].nickname, mine[i].type, mine[i].hp ,mine[i].pre_hp, mine[i].st);
                cnt++;
            }
            else {
                printf("%2d.  %s(%s) %s %d/%d - %d : 상태이상\n", i + 1, mine[i].name, mine[i].nickname, mine[i].type,mine[i].hp,mine[i].pre_hp, mine[i].st);
                deadPk[i+1]++;
            }
        }
        if (cnt > 0) {
            do {
                printf("누구를 내보내겠습니까?( 도망:0 ) \n >> ");
                scanf("%d", &num);
                if(deadPk[num]==1) printf("해당 포켓몬은 상태이상에 걸렸습니다.\n");
            } while (deadPk[num]==1);
            
            if(num == 0) main_game(mine);
            num -= 1;
            attack(mine, *enemy, num, 1); // 상대먼저 공격인걸로 시작
        }
        else {
            printf("존재하지 않음...\n");  printf("눈앞이 깜깜해졌다...\n"); wt.money -= 1000;
            main_game(mine);
        }
    }
}
void enter(){
    char input[5];
    while(1){
        gets(input); getchar();
        if(input[0]==0) break;
    }
}