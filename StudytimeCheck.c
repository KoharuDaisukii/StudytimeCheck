#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strlen
#include <ctype.h> // toupper, isalpha, isdigit
#include <sys/stat.h> // mkdir
#include <sys/types.h> // mkdir, lseek
#include <fcntl.h> // open
#include <dirent.h> // struct dirent
#include <unistd.h> // mkdir, chdir, write, lseek, dup(1,2)
#include <curses.h>
#include <time.h> // time, struct tm

#define USERS_INFO_DIR "users"
#define USERS_INFO_FILE "users.txt"
#define NO_GROUP "no_group"

#define MAX 11
#define ARROW_DOWN 2
#define ARROW_UP 3
#define ARROW_LEFT 4
#define ARROW_RIGHT 5

typedef struct Studyuser
{
	char user_ID[11];
	char group_ID[11];
	time_t signup;// 가입 일자
	time_t lastlogin;// 최종 접속 시간
	// 또 뭐 넣지
} Studyuser;


typedef struct timelog // 공부 시간 기록을 저장하는 구조체
{
	// Studyuser info; 
	char subject[30];
	time_t start_time; // 공부 시작 시간
	time_t finish_time; // 공부 종료 시간
	double studytime; // 공부 시간
} timelog;

DIR* login(); // login 성공: UID directory 포인터 return

void main_screen();
void menu2();
void menu2_screen(WINDOW* win);
void menu2_1(WINDOW* win); // 이름을 어칼까
void day_stats(WINDOW* win, int year, int month, int day);

// MENU 3 관련 함수 
void menu3();
void menu3_screen(WINDOW* win);
void menu3_join(WINDOW* win);
void menu3_leave(WINDOW* win);
void menu3_rank(WINDOW* win);

void main_screen();
void menu2();
void menu2_screen(WINDOW* win);
void menu2_1(WINDOW* win); // 이름을 어칼까
void day_stats(WINDOW* win, int year, int month, int day);

void menu4();
void menu5(DIR*);
void menu5_screen(WINDOW* win);
void menu5_deleteAccount(WINDOW* win, DIR*);

int rmdir_r(DIR* path);

int user_dead = 0; // 유저가 탈퇴했는지 인지하는 변수
int usersFd; // USERS_INFO_FILE file descriptor, 이건 users.txt 파일 내부에서 로그인한 유저의 정보를 계속 가리킬 예정
char UID[11]; // 전역 변수로 쓰는 게 편할 것 같음

int main(int argc, char* argv[])
{
	if (argc == 1) // 아이디 입력 안 함
	{
		fprintf(stderr, "아이디를 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(1);
	}
	else if (strlen(argv[1]) >= 11) // 아이디 10글자 이하로 입력 안 함
	{
		fprintf(stderr, "아이디는 10글자 이하로 입력해주세요.\n");
		exit(2);
	}
	else if (argc >= 3) // 쓸데없는 것 입력함
	{
		fprintf(stderr, "ID만 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(3);
	}

	strcpy(UID, argv[1]); // 
	DIR* UID_dirptr = login(UID); // 로그인해서 UID 이름의 폴더를 엶

	initscr();
	noecho();
	curs_set(0);

	char menu;
	while (1)
	{
		main_screen(UID);
		menu = getch();
		// if(menu == '1') menu1();
		if (menu == '2') menu2();
		if (menu == '3') menu3();
		if (menu == '4') menu4();
		if (menu == '5') menu5(UID_dirptr);
		if (menu == '6') break;
		if (user_dead == 1) break;
	}

	// delwin(main_screen);
	endwin();
	closedir(UID_dirptr); // 로그아웃
	close(usersFd);
	printf("%s님, Bye Bye!\n", UID);
	return 0;
}

DIR* login()
{
	DIR* dir_ptr;

	if (chdir(USERS_INFO_DIR) == -1) // 유저 정보가 담긴 디렉토리로 이동
	{
		if (mkdir(USERS_INFO_DIR, 0755) == -1 || chdir(USERS_INFO_DIR) == -1) // 없으면 만듦
		{
			perror(USERS_INFO_DIR);
			exit(4);
		}

	}

	int tempfd1, tempfd2;
	int user_exist = 0; //
	if ((tempfd1 = open(USERS_INFO_FILE, O_RDWR | O_CREAT | O_EXCL, 0777)) == -1) // 파일 만들기 시도
	{
		if ((tempfd2 = open(USERS_INFO_FILE, O_RDWR)) == -1) // 이미 있네
		{
			perror("open fd2");
			exit(9);
		}
		if ((usersFd = dup(tempfd2)) == -1) // usersFd에 tempfd2 복제
		{
			perror("dup2-1");
			exit(9);
		}
		close(tempfd2);
	}
	else // 파일 없었을 때(이 시점에서는 존재함)
	{
		if ((usersFd = dup(tempfd1)) == -1) // usersFd에 tempfd2 복제
		{
			perror("dup2-2");
			exit(9);
		}
		close(tempfd1);
	}

	Studyuser s_user;
	while (read(usersFd, &s_user, sizeof(Studyuser)) >= sizeof(Studyuser))
		if (strcmp(s_user.user_ID, UID) == 0) // 해당하는 UID의 유저가 있는지 탐색
		{
			user_exist = 1; // 있네
			break; // file pointer는 찾은 유저의 기록 바로 다음을 가리킴
		}

	// 한글 ID 입력하면 컷하는 기능도 필요할 듯
	if ((dir_ptr = opendir(UID)) == NULL || user_exist == 0) // UID 폴더 존재하는 지 확인, 그룹 생각 안 하고 일단 함
	{
		printf("ID가 존재하지 않습니다. 해당 ID로 가입하시겠습니까? (Y/N): ");
		char yesno = toupper(getchar());
		while (yesno != 'Y' && yesno != 'N') // y도 아니고 n도 아니면 반복
		{
			printf("Y 또는 N만 입력해라: ");
			yesno = toupper(getchar());
		}

		if (yesno == 'Y') // Y 입력
		{
			if (mkdir(UID, 0755) == -1) // UID 이름의 디렉토리 만듦 = 가입
			{
				perror("mkdir error");
				exit(6);
			}
			if ((dir_ptr = opendir(UID)) == NULL) // UID 디렉토리 열기
			{
				perror("안 열려요");
				exit(7);
			}

			// user_info 파일에 유저 정보 추가
			Studyuser newuser;
			strcpy(newuser.user_ID, UID);
			strcpy(newuser.group_ID, NO_GROUP);
			newuser.signup = time(NULL);
			newuser.lastlogin = time(NULL);
			lseek(usersFd, 0, SEEK_END);
			write(usersFd, &newuser, sizeof(Studyuser));
			// 추가 완료

			printf("가입 완료되었습니다. %s님 환영합니다.\n", UID);
			sleep(2); // 로딩하는 척
			return dir_ptr;
		}
		else
		{
			printf("\'가입하지 않기\'를 선택하셨습니다. 프로그램이 종료됩니다.\n");
			sleep(2);
			exit(0);
		}
	}

	// 로그인 시간 갱신
	s_user.lastlogin = time(NULL);
	lseek(usersFd, -sizeof(Studyuser), SEEK_CUR);
	write(usersFd, &s_user, sizeof(Studyuser));
	// 추가 완료

	return dir_ptr;
}

void main_screen()
{
	int x = 3;
	int  y = 7;

	//S
	move(y, x);
	printw("*");
	move(y, x + 1);
	printw("*");
	move(y, x + 2);
	printw("*");
	move(y, x + 3);
	printw("*");
	move(y, x + 4);
	printw("*");
	move(y + 1, x);
	printw("*");
	move(y + 2, x);
	printw("*");
	move(y + 2, x + 1);
	printw("*");
	move(y + 2, x + 2);
	printw("*");
	move(y + 2, x + 3);
	printw("*");
	move(y + 2, x + 4);
	printw("*");
	move(y + 3, x + 4);
	printw("*");
	move(y + 4, x + 4);
	printw("*");
	move(y + 4, x + 3);
	printw("*");
	move(y + 4, x + 2);
	printw("*");
	move(y + 4, x + 1);
	printw("*");
	move(y + 4, x);
	printw("*");

	//T
	move(y, x + 7);
	printw("*");
	move(y, x + 8);
	printw("*");
	move(y, x + 9);
	printw("*");
	move(y, x + 10);
	printw("*");
	move(y, x + 11);
	printw("*");
	move(y + 1, x + 9);
	printw("*");
	move(y + 2, x + 9);
	printw("*");
	move(y + 3, x + 9);
	printw("*");
	move(y + 4, x + 9);
	printw("*");

	//U
	move(y, x + 14);
	printw("*");
	move(y + 1, x + 14);
	printw("*");
	move(y + 2, x + 14);
	printw("*");
	move(y + 3, x + 14);
	printw("*");
	move(y + 4, x + 14);
	printw("*");
	move(y, x + 18);
	printw("*");
	move(y + 1, x + 18);
	printw("*");
	move(y + 2, x + 18);
	printw("*");
	move(y + 3, x + 18);
	printw("*");
	move(y + 4, x + 18);
	printw("*");
	move(y + 4, x + 15);
	printw("*");
	move(y + 4, x + 16);
	printw("*");
	move(y + 4, x + 17);
	printw("*");

	//D
	move(y, x + 21);
	printw("*");
	move(y + 1, x + 21);
	printw("*");
	move(y + 2, x + 21);
	printw("*");
	move(y + 3, x + 21);
	printw("*");
	move(y + 4, x + 21);
	printw("*");
	move(y, x + 22);
	printw("*");
	move(y, x + 23);
	printw("*");
	move(y, x + 24);
	printw("*");
	move(y + 4, x + 22);
	printw("*");
	move(y + 4, x + 23);
	printw("*");
	move(y + 4, x + 24);
	printw("*");
	move(y + 1, x + 25);
	printw("*");
	move(y + 2, x + 25);
	printw("*");
	move(y + 3, x + 25);
	printw("*");

	//Y
	move(y, x + 27);
	printw("*");
	move(y + 1, x + 28);
	printw("*");
	move(y + 2, x + 29);
	printw("*");
	move(y + 2, x + 30);
	printw("*");
	move(y + 2, x + 31);
	printw("*");
	move(y + 1, x + 32);
	printw("*");
	move(y, x + 33);
	printw("*");
	move(y + 3, x + 30);
	printw("*");
	move(y + 4, x + 30);
	printw("*");

	//2번째 줄 
	//T
	move(y + 6, x + 4);
	printw("*");
	move(y + 6, x + 5);
	printw("*");
	move(y + 6, x + 6);
	printw("*");
	move(y + 6, x + 7);
	printw("*");
	move(y + 6, x + 8);
	printw("*");
	move(y + 7, x + 6);
	printw("*");
	move(y + 8, x + 6);
	printw("*");
	move(y + 9, x + 6);
	printw("*");
	move(y + 10, x + 6);
	printw("*");

	//I
	move(y + 6, x + 12);
	printw("*");
	move(y + 6, x + 11);
	printw("*");
	move(y + 6, x + 13);
	printw("*");
	move(y + 7, x + 12);
	printw("*");
	move(y + 8, x + 12);
	printw("*");
	move(y + 9, x + 12);
	printw("*");
	move(y + 10, x + 12);
	printw("*");
	move(y + 10, x + 11);
	printw("*");
	move(y + 10, x + 13);
	printw("*");

	//M


	move(x, y + 40);
	printw("Welcome! %s!", UID);
	move(x, y + 41);
	printw("1. Studytime Measuring");
	move(x, y + 42);
	printw("2. Display Stats");
	move(x, y + 43);
	printw("3. Group");
	move(x, y + 44);
	printw("4. My profile");
	move(x, y + 45);
	printw("5. Settings");
	move(x, y + 46);
	printw("6. Exit");
	refresh();
}

void menu2()
{
	WINDOW* win = newwin(20, 60, 1, 1);

	char c;
	while (1)
	{
		menu2_screen(win);
		c = wgetch(win);
		if (c == 'q') break;
		if (c == '1') menu2_1(win);
		if (c == '2');
		if (c == '3');
		if (user_dead == 1) break;
	}

	wclear(win);
	wrefresh(win);
	delwin(win);
}

void menu2_screen(WINDOW* win)
{
	box(win, '|', '+');
	mvwprintw(win, 3, 2, "Display stats                       ");
	mvwprintw(win, 5, 2, "1. Today's Studytime                ");
	mvwprintw(win, 6, 2, "2. This week's Studytime            ");
	mvwprintw(win, 7, 2, "3. This month's Studytime           ");
	mvwprintw(win, 17, 2, "\'q\' to quit");
	wrefresh(win);
}

void menu2_1(WINDOW* win)
{
	time_t statdate = time(NULL);
	time_t today = statdate;
	struct tm* statdate_tm = localtime(&statdate);

	int today_year = statdate_tm->tm_year + 1900;
	int today_month = statdate_tm->tm_mon + 1;
	int today_day = statdate_tm->tm_mday;

	// 통계를 보여줄 년월일 변수
	int stat_year = statdate_tm->tm_year + 1900;
	int stat_month = statdate_tm->tm_mon + 1;
	int stat_day = statdate_tm->tm_mday;

	keypad(win, TRUE); // 방향키 받을 거임

	char c;
	while (1)
	{
		mvwprintw(win, 3, 2, "Display stats (Arrow keys for control)");
		if (stat_year == today_year && stat_month == today_month && stat_day == today_day)
			mvwprintw(win, 5, 2, "Studytime during Today(%04d-%02d-%02d)", stat_year, stat_month, stat_day);
		else
			mvwprintw(win, 5, 2, "Studytime of %04d-%02d-%02d           ", stat_year, stat_month, stat_day);
		mvwprintw(win, 6, 2, "                         ");
		mvwprintw(win, 7, 2, "                         ");
		day_stats(win, stat_year, stat_month, stat_day);
		wrefresh(win);

		c = wgetch(win);
		if (c == 'q') break;
		if (c == ARROW_DOWN || c == ARROW_LEFT)
		{
			statdate -= 86400;
			statdate_tm = localtime(&statdate);
			stat_year = statdate_tm->tm_year + 1900;
			stat_month = statdate_tm->tm_mon + 1;
			stat_day = statdate_tm->tm_mday;
		}
		if (statdate < today && (c == ARROW_UP || c == ARROW_RIGHT))
		{
			statdate += 86400;
			statdate_tm = localtime(&statdate);
			stat_year = statdate_tm->tm_year + 1900;
			stat_month = statdate_tm->tm_mon + 1;
			stat_day = statdate_tm->tm_mday;
		}
	}
	wclear(win);
}

void day_stats(WINDOW* win, int year, int month, int day)
{
	char statfile[256];

	sprintf(statfile, "%04d%02d%02d.txt", year, month, day);
	mvwprintw(win, 6, 20, "%s", statfile);

	char UID_dir[256];
	sprintf(UID_dir, "%s/%s", ".", UID);
	mvwprintw(win, 6, 2, "%s", UID_dir);
	if (chdir(UID_dir) == -1)
	{
		perror("chdir");
		exit(21);
	}

	int fd1;
	if ((fd1 = creat(statfile, 0777)) == -1)
	{
		perror("open");
		exit(22);
	}

	srand(time(NULL));
	timelog templog;
	strcpy(templog.subject, "C Language");
	templog.start_time = 0;
	templog.finish_time = rand() % 30 + 6;
	templog.studytime = (double)templog.finish_time - templog.start_time;
	write(fd1, &templog, sizeof(timelog));

	timelog templog2;
	strcpy(templog2.subject, "System Programming");
	templog2.start_time = 0;
	templog2.finish_time = rand() % 30 + 6;
	templog2.studytime = (double)templog2.finish_time - templog2.start_time;
	write(fd1, &templog2, sizeof(timelog));
	close(fd1);

	int fd2;
	if ((fd2 = open(statfile, O_RDONLY)) == -1)
	{
		perror("open");
		exit(22);
	}

	timelog templog3;
	int j = 0;
	mvwprintw(win, 9, 2, "--------------------------------------");
	mvwprintw(win, 12, 2, "--------------------------------------");
	while (read(fd2, &templog3, sizeof(timelog)) >= sizeof(timelog))
	{
		mvwprintw(win, 8 + 3 * j, 2, "%s studytime: %.0f minutes         ", templog3.subject, templog3.studytime * 30);
		for (int i = 0; i < templog3.studytime; i++)
			mvwprintw(win, 9 + 3 * j, 2 + i, "%%");
		j++;
	}

	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
	close(fd2);
}

void menu3() {
	//initscr();
	noecho();
	//wclear(win);
	// make new window
	WINDOW* win = newwin(50, 100, 1, 1);

	char menu;
	while (1) {
		menu3_screen(win);
		menu = getch();
		if (menu == '1') menu3_join(win);
		if (menu == '2') menu3_leave(win);
		//if(menu=='3') menu3_rank(win);
		if (menu == '4') {
			break;
		}
	}
	wclear(win);
	wrefresh(win);
	delwin(win);
	return;
}
void menu3_join(WINDOW* win) {

	wclear(win);
	box(win, '|', '_');
	wrefresh(win);

	echo();
	int user_flag = 0;
	char* groupid = malloc(sizeof(char*) * MAX);
	char* userid = malloc(sizeof(char*) * MAX);

	mvwprintw(win, 3, 2, "Insert your USER ID : ");
	mvwgetstr(win, 3, 24, userid);
	//If the user has a group id, should we tell users to leave the group and rejoin to another group?
	mvwprintw(win, 6, 2, "Enter the GROUP ID that you want to join : ");
	mvwgetstr(win, 6, 45, groupid);

	int ufd = usersFd;
	int read_st = 0;
	Studyuser j_user;

	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		if (strcmp(j_user.user_ID, userid) == 0) {
			user_flag = 1; // userid is found
			strcpy(j_user.group_ID, groupid); // GROUP ID is copy to j_user
			lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
			write(ufd, &j_user, sizeof(Studyuser)); // replacing
			wrefresh(win);
		}
	}
	if (user_flag == 1) {
		mvwprintw(win, 8, 2, "You join in to <%s> !", groupid);
		mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
		wrefresh(win);
	}
	else {
		mvwprintw(win, 8, 2, "There is NO USER ID !!!!");
		wrefresh(win);
	}

	char menu;
	while ((menu = getch()) != 'q');

	free(groupid);
	free(userid);
	wclear(win);
	wrefresh(win);
	return;
}
void menu3_leave(WINDOW* win) {

	wclear(win);
	box(win, '|', '_');
	wrefresh(win);

	echo();
	int user_flag = 0;
	char* userid = malloc(sizeof(char*) * MAX);
	char yn;
	char groupid[MAX];

	mvwprintw(win, 3, 2, "Insert your USER ID : ");
	mvwgetstr(win, 3, 24, userid);

	mvwprintw(win, 5, 2, "Do you want to leave this group ? (y/n) : ");
	yn = mvwgetch(win, 5, 45);

	if (yn == 'y') {
		int ufd = usersFd;
		int read_st = 0;
		Studyuser j_user;

		lseek(ufd, 0, SEEK_SET);
		while (read(ufd, &j_user, sizeof(Studyuser))) {
			if (strcmp(j_user.user_ID, userid) == 0) {
				if (strcmp(j_user.group_ID, NO_GROUP) != 0) { // when user has a group
					user_flag = 1; // userid is found
					strcpy(groupid, j_user.group_ID); //copying group id before changing
					strcpy(j_user.group_ID, NO_GROUP); // GROUP ID is copy to j_user
					lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
					write(ufd, &j_user, sizeof(Studyuser)); // replacing
					wrefresh(win);
				}
				else {
					user_flag = 2;
				}
			}
		}
		if (user_flag == 1) {
			mvwprintw(win, 8, 2, "You just left in to <%s> !", groupid);
			mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
			wrefresh(win);
		}
		else if (user_flag == 0) {
			mvwprintw(win, 8, 2, "There is NO USER ID !!!!");
			mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
			wrefresh(win);
		}
		else if (user_flag == 2) {
			mvwprintw(win, 8, 2, "%s ! You already don't have a GROUP !!!", userid);
			mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
			wrefresh(win);

		}
	}
	else {
		mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
		wrefresh(win);
	}

	char menu;
	while ((menu = getch()) != 'q');

	free(userid);
	wclear(win);
	wrefresh(win);
	return;

}
void menu3_rank(WINDOW* win) {

	wclear(win);
	box(win, '|', '_');
	wrefresh(win);

	echo();
	int user_flag = 0;
	char* groupid = malloc(sizeof(char*) * MAX);

	mvwprintw(win, 3, 2, "Insert your GROUP ID : ");
	mvwgetstr(win, 3, 24, groupid);

	int ufd = usersFd;
	Studyuser j_user;
	Studyuser rank_user[50];
	int index = 0;
	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		if (strcmp(j_user.group_ID, groupid) == 0) {// searching other users that match user's group
			rank_user[index] = j_user; // 공부시간을 어떻게 가지고 올지 ??
			index++;
		}
	}
	if (user_flag == 1) {
		mvwprintw(win, 8, 2, "You just left in to <%s> !", groupid);
		mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
		wrefresh(win);
	}
	else if (user_flag == 0) {
		mvwprintw(win, 8, 2, "There is NO USER ID !!!!");
		mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
		wrefresh(win);
	}
	else if (user_flag == 2) {
		mvwprintw(win, 8, 2, "%s ! You already don't have a GROUP !!!", groupid);
		mvwprintw(win, 9, 2, "If you want to go back, press 'q' !");
		wrefresh(win);

	}


	char menu;
	while ((menu = getch()) != 'q');

	free(groupid);
	wclear(win);
	wrefresh(win);
	return;

}
void menu3_screen(WINDOW* win) {
	int x = 3;
	int y = 7;

	box(win, '|', '_');

	//G
	mvwprintw(win, x, y, "*");
	mvwprintw(win, x, y + 1, "*");
	mvwprintw(win, x, y + 2, "*");
	mvwprintw(win, x + 1, y - 1, "*");
	mvwprintw(win, x + 2, y - 1, "*");
	mvwprintw(win, x + 2, y + 1, "*");
	mvwprintw(win, x + 2, y + 2, "*");
	mvwprintw(win, x + 2, y + 3, "*");
	mvwprintw(win, x + 3, y - 1, "*");
	mvwprintw(win, x + 3, y + 3, "*");
	mvwprintw(win, x + 4, y, "*");
	mvwprintw(win, x + 4, y + 1, "*");
	mvwprintw(win, x + 4, y + 2, "*");

	//R
	mvwprintw(win, x, y + 5, "*");
	mvwprintw(win, x + 1, y + 5, "*");
	mvwprintw(win, x + 2, y + 5, "*");
	mvwprintw(win, x + 3, y + 5, "*");
	mvwprintw(win, x + 4, y + 5, "*");
	mvwprintw(win, x, y + 6, "*");
	mvwprintw(win, x, y + 7, "*");
	mvwprintw(win, x + 1, y + 8, "*");
	mvwprintw(win, x + 2, y + 6, "*");
	mvwprintw(win, x + 2, y + 7, "*");
	mvwprintw(win, x + 3, y + 7, "*");
	mvwprintw(win, x + 4, y + 8, "*");

	//O
	mvwprintw(win, x, y + 11, "*");
	mvwprintw(win, x, y + 12, "*");
	mvwprintw(win, x, y + 13, "*");
	mvwprintw(win, x + 1, y + 10, "*");
	mvwprintw(win, x + 2, y + 10, "*");
	mvwprintw(win, x + 3, y + 10, "*");
	mvwprintw(win, x + 4, y + 11, "*");
	mvwprintw(win, x + 4, y + 12, "*");
	mvwprintw(win, x + 4, y + 13, "*");
	mvwprintw(win, x + 3, y + 14, "*");
	mvwprintw(win, x + 2, y + 14, "*");
	mvwprintw(win, x + 1, y + 14, "*");

	//U
	mvwprintw(win, x, y + 16, "*");
	mvwprintw(win, x + 1, y + 16, "*");
	mvwprintw(win, x + 2, y + 16, "*");
	mvwprintw(win, x + 3, y + 16, "*");
	mvwprintw(win, x + 4, y + 17, "*");
	mvwprintw(win, x + 4, y + 18, "*");
	mvwprintw(win, x + 4, y + 19, "*");
	mvwprintw(win, x + 3, y + 20, "*");
	mvwprintw(win, x + 2, y + 20, "*");
	mvwprintw(win, x + 1, y + 20, "*");
	mvwprintw(win, x, y + 20, "*");

	//P
	mvwprintw(win, x, y + 22, "*");
	mvwprintw(win, x, y + 23, "*");
	mvwprintw(win, x, y + 24, "*");
	mvwprintw(win, x, y + 25, "*");
	mvwprintw(win, x + 1, y + 22, "*");
	mvwprintw(win, x + 1, y + 26, "*");
	mvwprintw(win, x + 2, y + 22, "*");
	mvwprintw(win, x + 2, y + 23, "*");
	mvwprintw(win, x + 2, y + 24, "*");
	mvwprintw(win, x + 2, y + 25, "*");
	mvwprintw(win, x + 3, y + 22, "*");
	mvwprintw(win, x + 4, y + 22, "*");

	//M
	mvwprintw(win, x + 7, y + 2, "*");
	mvwprintw(win, x + 8, y + 2, "*");
	mvwprintw(win, x + 9, y + 2, "*");
	mvwprintw(win, x + 10, y + 2, "*");
	mvwprintw(win, x + 6, y + 3, "*");
	mvwprintw(win, x + 7, y + 4, "*");
	mvwprintw(win, x + 8, y + 4, "*");
	mvwprintw(win, x + 6, y + 5, "*");
	mvwprintw(win, x + 7, y + 6, "*");
	mvwprintw(win, x + 8, y + 6, "*");
	mvwprintw(win, x + 9, y + 6, "*");
	mvwprintw(win, x + 10, y + 6, "*");

	//E
	mvwprintw(win, x + 6, y + 8, "*");
	mvwprintw(win, x + 6, y + 9, "*");
	mvwprintw(win, x + 6, y + 10, "*");
	mvwprintw(win, x + 6, y + 11, "*");
	mvwprintw(win, x + 7, y + 8, "*");
	mvwprintw(win, x + 8, y + 8, "*");
	mvwprintw(win, x + 8, y + 9, "*");
	mvwprintw(win, x + 8, y + 10, "*");
	mvwprintw(win, x + 8, y + 11, "*");
	mvwprintw(win, x + 9, y + 8, "*");
	mvwprintw(win, x + 10, y + 8, "*");
	mvwprintw(win, x + 10, y + 9, "*");
	mvwprintw(win, x + 10, y + 10, "*");
	mvwprintw(win, x + 10, y + 11, "*");

	//N
	mvwprintw(win, x + 6, y + 13, "*");
	mvwprintw(win, x + 7, y + 13, "*");
	mvwprintw(win, x + 8, y + 13, "*");
	mvwprintw(win, x + 9, y + 13, "*");
	mvwprintw(win, x + 10, y + 13, "*");
	mvwprintw(win, x + 7, y + 14, "*");
	mvwprintw(win, x + 8, y + 15, "*");
	mvwprintw(win, x + 9, y + 16, "*");
	mvwprintw(win, x + 6, y + 17, "*");
	mvwprintw(win, x + 7, y + 17, "*");
	mvwprintw(win, x + 8, y + 17, "*");
	mvwprintw(win, x + 9, y + 17, "*");
	mvwprintw(win, x + 10, y + 17, "*");

	//U
	mvwprintw(win, x + 6, y + 19, "*");
	mvwprintw(win, x + 7, y + 19, "*");
	mvwprintw(win, x + 8, y + 19, "*");
	mvwprintw(win, x + 9, y + 19, "*");
	mvwprintw(win, x + 10, y + 20, "*");
	mvwprintw(win, x + 10, y + 21, "*");
	mvwprintw(win, x + 10, y + 22, "*");
	mvwprintw(win, x + 6, y + 23, "*");
	mvwprintw(win, x + 7, y + 23, "*");
	mvwprintw(win, x + 8, y + 23, "*");
	mvwprintw(win, x + 9, y + 23, "*");

	mvwprintw(win, x + 14, y, "You selected Group Menu !");
	mvwprintw(win, x + 15, y, "Select group menu that you want.");
	mvwprintw(win, x + 17, y, "1. Group Join");
	mvwprintw(win, x + 18, y, "2. Group Leave");
	mvwprintw(win, x + 19, y, "3. Group Ranking");
	mvwprintw(win, x + 20, y, "4. Go back");
	wrefresh(win);
}

void menu4()
{
	WINDOW* win = newwin(20, 60, 1, 1);
	box(win, '|', '+');

	int ufd = usersFd;
	Studyuser s_user; // 내 정보 읽어오기
	lseek(ufd, -sizeof(Studyuser), SEEK_CUR);
	read(ufd, &s_user, sizeof(Studyuser));

	struct tm* tm_ptr;

	mvwprintw(win, 3, 2, "%s\'s profile", s_user.user_ID);
	mvwprintw(win, 5, 2, "User ID: %s", s_user.user_ID);
	mvwprintw(win, 6, 2, "Group ID: %s", s_user.group_ID);

	tm_ptr = localtime(&(s_user.signup)); // time_t-> struct tm
	mvwprintw(win, 8, 2, "Sign up time: %25s", asctime(tm_ptr)); // struct tm -> human_readable
	tm_ptr = localtime(&(s_user.lastlogin)); // time_t -> struct tm
	mvwprintw(win, 9, 2, "Last login time: %25s", asctime(tm_ptr)); // struct tm -> human_readable
	mvwprintw(win, 17, 2, "%s", "\'q\' to quit");
	wrefresh(win);

	char quit = '0';
	while ((quit = wgetch(win)) != 'q'); // q 누르면 나가기
	clear();
	wrefresh(win);
	delwin(win);

	return;
}


/*
typedef struct
{
	short id; // device id
	int x, y, z; // 마우스 커서 위치q
	mmask_t bstate; // 버튼의 상태 비트
} MEVENT;
*/
void menu5(DIR* dir_ptr)
{
	WINDOW* win = newwin(20, 60, 1, 1);

	char c;
	while (1)
	{
		menu5_screen(win);
		c = wgetch(win);
		if (c == 'q') break;
		if (c == '1');
		if (c == '2');
		if (c == '3') menu5_deleteAccount(win, dir_ptr); // 3 누르면 계정 삭제할지 말지 선택
		if (user_dead == 1) break;
	}
	wclear(win);
	wrefresh(win);
	delwin(win);
}

void menu5_screen(WINDOW* win)
{
	box(win, '|', '+');
	mvwprintw(win, 3, 2, "Settings");
	mvwprintw(win, 5, 2, "1. ??");
	mvwprintw(win, 6, 2, "2. ???");
	mvwprintw(win, 7, 2, "3. Delete account");
	mvwprintw(win, 17, 2, "\'q\' to quit");
	wrefresh(win);
}

void menu5_deleteAccount(WINDOW* win, DIR* uid_dirptr)
{
	char yesno;
	curs_set(1);
	mvwprintw(win, 17, 2, "             ");
	mvwprintw(win, 9, 2, "Delete your account from StudytimeCheck? (Y/N) "); wrefresh(win);
	while (1)
	{
		yesno = wgetch(win); // 삭제하려면 y 입력
		yesno = toupper(yesno);
		if (yesno == 'Y')
		{
			mvwprintw(win, 11, 2, "Enter your ID if you really want to leave.");
			wrefresh(win);
			mvwprintw(win, 13, 3, "          )");
			mvwprintw(win, 13, 2, "(");
			wrefresh(win);
			char input[11] = "\0", input_c;
			int i = 0;
			while (1)
			{
				input_c = wgetch(win);
				if (isalpha(input_c) || isdigit(input_c)) // 알파벳, 숫자만 허용
				{
					if (i < 10) // 10글자 이상 입력 blocking
					{
						mvwprintw(win, 13, 3 + i, "%c", input_c);
						input[i++] = input_c;
						input[i] = '\0';
					}
				}
				if ((input_c == '\b' || input_c == 127) && i > 0) // 백스페이스로 0글자 이하로 가는 거 blocking
				{
					mvwprintw(win, 13, 3 + --i, " ");
					move(13, 1 + i);
				}
				if (input_c == '\n') // 엔터 입력
				{
					if (strcmp(input, UID) == 0) // 올바르게 입력
					{
						// 기존의 ID 정보를 DEAD_USER로 덮어쓰기
						Studyuser left_user;
						strcpy(left_user.user_ID, "DEAD_USER");
						strcpy(left_user.group_ID, NO_GROUP);
						left_user.signup = 0;
						left_user.lastlogin = 0;
						lseek(usersFd, -sizeof(Studyuser), SEEK_CUR);
						write(usersFd, &left_user, sizeof(Studyuser));

						// ID 디렉토리와 내부 파일들 전부 삭제
						if (rmdir_r(uid_dirptr) == -1)
						{
							perror("rmdir");
							exit(50);
						}
						mvwprintw(win, 13, 2, "Deactivated your account. See you Again...");
						wrefresh(win);
						sleep(2);
						user_dead = 1; // 유저가 탈퇴했음.
					}
					break;
				}
				wrefresh(win);
			}
			break;
		}
		if (yesno == 'N')
		{
			mvwprintw(win, 9, 2, "                                               ");
			wrefresh(win);
			break;
		}
		wrefresh(win);
	}
	wclear(win);
	wrefresh(win);
	noecho(); cbreak(); curs_set(0);
	return;
}

int rmdir_r(DIR* rm_dirptr) // 유저 디렉토리 remove하는 함수, 괜히 DIR*로 받았나
{
	struct dirent* file = NULL;
	char path[256];
	char filename[1024];
	getcwd(path, sizeof(path));

	while ((file = readdir(rm_dirptr)) != NULL)
	{
		if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
			continue;
		sprintf(filename, "%s/%s", path, file->d_name);
		if (unlink(filename) == -1)
		{
			perror("unlink");
			exit(60);
		}
	}
	// closedir(rm_dirptr);
	return rmdir(UID);
}

