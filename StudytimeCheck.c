#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strlen
#include <ctype.h> // toupper, isalpha, isdigit
#include <sys/stat.h> // mkdir
#include <sys/types.h> // mkdir, lseek
#include <sys/wait.h> // wait
#include <fcntl.h> // open
#include <dirent.h> // struct dirent
#include <unistd.h> // mkdir, chdir, write, lseek, dup(1,2)
#include <curses.h>
#include <time.h> // time, struct tm
#include "varlib.h"

#define JANUARY 0
#define DECEMBER 11
#define SECONDS_PER_DAY 86400
#define MAX 11
#define ARROW_DOWN 2
#define ARROW_UP 3
#define ARROW_LEFT 4
#define ARROW_RIGHT 5

DIR* login(); // login 성공: UID directory 포인터 return

void main_screen();
void menu2();
void menu2_screen(WINDOW* win);
void menu2_1(WINDOW* win); // 이름을 어칼까
void day_stats(WINDOW* win, int year, int month, int day);
void menu2_2(WINDOW* win);
void week_stats(WINDOW* win, time_t today);
void menu2_3(WINDOW* win);
void month_stats(WINDOW* win, struct tm statmonth_tm);

// MENU 3 관련 함수 
void menu3();
void menu3_create(WINDOW* win);
void menu3_screen(WINDOW* win);
void menu3_join(WINDOW* win);
void menu3_leave(WINDOW* win);
void menu3_rank(WINDOW* win);

void menu4(DIR*);
void menu4_screen(WINDOW* win);
void menu4_profile(WINDOW* win);
void menu4_help(WINDOW* win);
void menu4_deleteAccount(WINDOW* win, DIR*);

int rmdir_r(DIR* path);

int user_dead = 0; // 유저가 탈퇴했는지 인지하는 변수
int usersFd; // USERS_INFO_FILE file descriptor, 이건 users.txt 파일 내부에서 로그인한 유저의 정보를 계속 가리킬 예정
char UID[11]; // 전역 변수로 쓰는 게 편할 것 같음

int main(int argc, char* argv[])
{
	DIR* UID_dirptr = login(argc, argv); // 로그인해서 UID 이름의 폴더를 엶

	char menu;
	while (1)
	{
		main_screen(UID);
		menu = getch();
		// if(menu == '1') menu1();
		if (menu == '2') menu2();
		if (menu == '3') menu3();
		if (menu == '4') menu4(UID_dirptr);
		if (menu == '5') break;
		if (user_dead == 1) break;
	}
	
	unsetup();
	return 0;
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
	move(x + 1, y + 40);
	printw("1. Studytime Measuring");
	move(x + 2, y + 40);
	printw("2. Display Stats");
	move(x + 3, y + 40);
	printw("3. Group");
	move(x + 4, y + 40);
	printw("4. Settings");
	move(x + 5, y + 40);
	printw("5. Exit");
	refresh();
}

void menu2()
{
	WINDOW* win = newwin(34, 60, 1, 1);

	char c;
	while (1)
	{
		menu2_screen(win);
		c = wgetch(win);
		if (c == 'q') break;
		if (c == '1') menu2_1(win);
		if (c == '2') menu2_2(win);
		if (c == '3') menu2_3(win);
		if (user_dead == 1) break;
	}

	wclear(win);
	wrefresh(win);
	delwin(win);
}

void menu2_screen(WINDOW* win)
{
	wclear(win);
	box(win, '|', '-');
	mvwprintw(win, 3, 2, "Display stats                       ");
	mvwprintw(win, 6, 2, "1. Today's Studytime                ");
	mvwprintw(win, 8, 2, "2. This week's Studytime            ");
	mvwprintw(win, 10, 2, "3. This month's Studytime           ");
	mvwprintw(win, 30, 2, "\'q\' to quit");
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
		mvwprintw(win, 10, 2, "                         ");
		day_stats(win, stat_year, stat_month, stat_day);
		wrefresh(win);

		c = wgetch(win);
		if (c == 'q') break;
		if (c == ARROW_DOWN || c == ARROW_LEFT)
		{
			statdate -= SECONDS_PER_DAY;
			statdate_tm = localtime(&statdate);
			stat_year = statdate_tm->tm_year + 1900;
			stat_month = statdate_tm->tm_mon + 1;
			stat_day = statdate_tm->tm_mday;
		}
		if (statdate < today && (c == ARROW_UP || c == ARROW_RIGHT))
		{
			statdate += SECONDS_PER_DAY;
			statdate_tm = localtime(&statdate);
			stat_year = statdate_tm->tm_year + 1900;
			stat_month = statdate_tm->tm_mon + 1;
			stat_day = statdate_tm->tm_mday;
		}
	}
	keypad(win, FALSE);
	wclear(win);
}

void day_stats(WINDOW* win, int year, int month, int day)
{
	char statfile[256];

	sprintf(statfile, "%04d%02d%02d.txt", year, month, day);


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
	timelog templog = { 0, };
	strcpy(templog.subject, "C Language");
	templog.start_time = 0;
	templog.finish_time = rand() % 10800 + 2;
	templog.studytime = (double)templog.finish_time - templog.start_time;
	write(fd1, &templog, sizeof(timelog));

	timelog templog2 = { 0, };
	strcpy(templog2.subject, "System Programming");
	templog2.start_time = 0;
	templog2.finish_time = rand() % 10800 + 2;
	templog2.studytime = (double)templog2.finish_time - templog2.start_time;
	write(fd1, &templog2, sizeof(timelog));
	close(fd1);

	int fd2;
	double total = 0.0;
	if ((fd2 = open(statfile, O_RDONLY)) == -1)
	{
		perror("open");
		exit(22);
	}

	timelog templog3;
	int j = 0;
	mvwprintw(win, 9, 2, "|----------------------------------------------------|");
	mvwprintw(win, 12, 2, "|----------------------------------------------------|");
	while (read(fd2, &templog3, sizeof(timelog)) >= sizeof(timelog))
	{
		total += templog3.studytime;
		mvwprintw(win, 8 + 3 * j, 2, "%s studytime: %.0f seconds         ", templog3.subject, templog3.studytime);
		for (int i = 0; i < templog3.studytime / 210; i++)
			mvwprintw(win, 9 + 3 * j, 3 + i, "%%");
		j++;
	}

	mvwprintw(win, 6, 2, "Total studytime: %.0f seconds", total);

	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
	close(fd2);
}

void menu2_2(WINDOW* win)
{
	time_t endweek = time(NULL);
	time_t today = endweek;
	time_t startweek = endweek - (SECONDS_PER_DAY * 6);
	// localtime은 static data구나
	struct tm endweek_tm; localtime_r(&endweek, &endweek_tm);
	struct tm startweek_tm; localtime_r(&startweek, &startweek_tm);

	// int today_year = endweek_tm->tm_year + 1900;
	// int today_month = endweek_tm->tm_mon + 1;
	// int today_day = endweek_tm->tm_mday;

	keypad(win, TRUE); // 방향키 받을 거임

	char c;
	while (1)
	{
		mvwprintw(win, 3, 2, "Display stats (Arrow keys for control)");
		mvwprintw(win, 5, 2, "Studytime of %04d-%02d-%02d ~ %04d-%02d-%02d", startweek_tm.tm_year + 1900, startweek_tm.tm_mon + 1, startweek_tm.tm_mday, endweek_tm.tm_year + 1900, endweek_tm.tm_mon + 1, endweek_tm.tm_mday);
		mvwprintw(win, 6, 2, "                         ");
		mvwprintw(win, 10, 2, "                         ");
		week_stats(win, endweek);
		wrefresh(win);

		c = wgetch(win);
		if (c == 'q') break;
		if (c == ARROW_DOWN || c == ARROW_LEFT)
		{
			endweek -= SECONDS_PER_DAY;
			startweek -= SECONDS_PER_DAY;
		}
		if (endweek < today && (c == ARROW_UP || c == ARROW_RIGHT))
		{
			endweek += SECONDS_PER_DAY;
			startweek += SECONDS_PER_DAY;
		}
		localtime_r(&endweek, &endweek_tm);
		localtime_r(&startweek, &startweek_tm);
	}
	keypad(win, FALSE);
	wclear(win);
}

void week_stats(WINDOW* win, time_t today)
{
	char UID_dir[256];
	sprintf(UID_dir, "%s/%s", ".", UID);
	// mvwprintw(win, 6, 2, "%s", UID_dir);
	if (chdir(UID_dir) == -1)
	{
		perror("chdir");
		exit(21);
	}

	timelog weeklog[7];
	int year, month, day;
	double total = 0.0;
	for (int week_i = 6; week_i >= 0; week_i--)
	{
		char statfile[15];
		struct tm today_tm; localtime_r(&today, &today_tm);
		year = today_tm.tm_year + 1900;
		month = today_tm.tm_mon + 1;
		day = today_tm.tm_mday;
		sprintf(statfile, "%04d%02d%02d.txt", year, month, day);

		weeklog[week_i].studytime = 0.0;
		int fd;
		if ((fd = open(statfile, O_RDONLY)) == -1)
		{
			mvwprintw(win, 8 + week_i * 3, 2, "%04d-%02d-%02d: %5.0f seconds", year, month, day, 0);
			mvwprintw(win, 9 + week_i * 3, 2, "|----------------------------------------------------|");
			today -= SECONDS_PER_DAY;
			continue;
		}

		timelog templog;
		while (read(fd, &templog, sizeof(timelog)) >= sizeof(timelog))
		{
			weeklog[week_i].studytime += templog.studytime;
			total += templog.studytime;
		}
		close(fd);

		mvwprintw(win, 8 + week_i * 3, 2, "%04d-%02d-%02d: %5.0f seconds", year, month, day, weeklog[week_i].studytime);
		mvwprintw(win, 9 + week_i * 3, 2, "|----------------------------------------------------|");
		for (int i = 0; i < weeklog[week_i].studytime / 1000; i++)
			mvwprintw(win, 9 + week_i * 3, 3 + i, "%%");
		today -= SECONDS_PER_DAY;
	}
	mvwprintw(win, 6, 2, "Total studytime: %.0f seconds", total);

	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
}

void menu2_3(WINDOW* win)
{
	time_t statmonth = time(NULL);
	time_t today = statmonth;
	// localtime은 static data구나
	struct tm statmonth_tm;
	// int today_year = endweek_tm->tm_year + 1900;
	// int today_month = endweek_tm->tm_mon + 1;
	// int today_day = endweek_tm->tm_mday;

	keypad(win, TRUE); // 방향키 받을 거임

	char c;
	while (1)
	{
		localtime_r(&statmonth, &statmonth_tm);
		mvwprintw(win, 3, 2, "Display stats (Arrow keys for control)");
		mvwprintw(win, 5, 20, "                       ");
		switch (statmonth_tm.tm_mon + 1)
		{
		case 1:
			mvwprintw(win, 5, 2, "Studytime of January, %d", statmonth_tm.tm_year + 1900);
			break;
		case 2:
			mvwprintw(win, 5, 2, "Studytime of February, %d", statmonth_tm.tm_year + 1900);
			break;
		case 3:
			mvwprintw(win, 5, 2, "Studytime of March, %d", statmonth_tm.tm_year + 1900);
			break;
		case 4:
			mvwprintw(win, 5, 2, "Studytime of April, %d", statmonth_tm.tm_year + 1900);
			break;
		case 5:
			mvwprintw(win, 5, 2, "Studytime of May, %d", statmonth_tm.tm_year + 1900);
			break;
		case 6:
			mvwprintw(win, 5, 2, "Studytime of June, %d", statmonth_tm.tm_year + 1900);
			break;
		case 7:
			mvwprintw(win, 5, 2, "Studytime of July, %d", statmonth_tm.tm_year + 1900);
			break;
		case 8:
			mvwprintw(win, 5, 2, "Studytime of August, %d", statmonth_tm.tm_year + 1900);
			break;
		case 9:
			mvwprintw(win, 5, 2, "Studytime of September, %d", statmonth_tm.tm_year + 1900);
			break;
		case 10:
			mvwprintw(win, 5, 2, "Studytime of October, %d", statmonth_tm.tm_year + 1900);
			break;
		case 11:
			mvwprintw(win, 5, 2, "Studytime of November, %d", statmonth_tm.tm_year + 1900);
			break;
		case 12:
			mvwprintw(win, 5, 2, "Studytime of December, %d", statmonth_tm.tm_year + 1900);
			break;
		}

		mvwprintw(win, 6, 2, "                         ");
		mvwprintw(win, 10, 2, "                         ");
		month_stats(win, statmonth_tm);
		wrefresh(win);

		c = wgetch(win);
		if (c == 'q') break;
		if (c == ARROW_DOWN || c == ARROW_LEFT)
		{
			if (statmonth_tm.tm_mon > JANUARY) // 2월~12월
				statmonth_tm.tm_mon--;
			else // 1월
			{
				statmonth_tm.tm_year--;
				statmonth_tm.tm_mon = DECEMBER; // 12월
			}
		}
		if (statmonth < today && (c == ARROW_UP || c == ARROW_RIGHT))
		{
			if (statmonth_tm.tm_mon < DECEMBER) // 1월~11월
				statmonth_tm.tm_mon++;
			else // 12월
			{
				statmonth_tm.tm_year++;
				statmonth_tm.tm_mon = JANUARY;
			}
		}
		statmonth = mktime(&statmonth_tm);
	}
	keypad(win, FALSE);
	wclear(win);
}

void month_stats(WINDOW* win, struct tm statmonth_tm)
{
	char UID_dir[256];
	sprintf(UID_dir, "%s/%s", ".", UID);
	// mvwprintw(win, 6, 2, "%s", UID_dir);

	if (chdir(UID_dir) == -1)
	{
		perror("chdir");
		exit(21);
	}

	// 구조체 배열은 static 한 건가?
	timelog subjectlog[7] = { 0, }; // 과목별 로그
	int year, month, day;
	year = statmonth_tm.tm_year + 1900;
	month = statmonth_tm.tm_mon + 1;
	double total = 0.0;
	int day_i, subject_count = 0;

	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		day_i = 31;
		break;
	case 2:
		day_i = 29;
		break;
	default:
		day_i = 30;
	}

	for (; day_i >= 1; day_i--)
	{
		char statfile[15];
		day = day_i;
		sprintf(statfile, "%04d%02d%02d.txt", year, month, day);

		memset(subjectlog[subject_count].subject, 0, sizeof(char) * 30);
		subjectlog[subject_count].studytime = 0.0;
		int fd;
		if ((fd = open(statfile, O_RDONLY)) == -1)
			continue;

		timelog templog;
		while (read(fd, &templog, sizeof(timelog)) >= sizeof(timelog))
		{
			int subject_i;
			total += templog.studytime;
			for (subject_i = 0; subject_i < subject_count; subject_i++)
			{
				if (strcmp(subjectlog[subject_i].subject, templog.subject) == 0)
				{
					subjectlog[subject_i].studytime += templog.studytime;
					break;
				}
			}
			if (subject_i != subject_count)
				continue;
			strcpy(subjectlog[subject_count].subject, templog.subject);
			subjectlog[subject_count++].studytime += templog.studytime;
		}
		close(fd);
	}
	mvwprintw(win, 6, 2, "Total studytime: %.0f seconds       ", total);
	for (int subject_i = 0; subject_i < subject_count; subject_i++)
	{
		mvwprintw(win, 8 + subject_i * 3, 2, "%d. %s", subject_i + 1, subjectlog[subject_i].subject);
		mvwprintw(win, 9 + subject_i * 3, 2, "|------------------------------------- %7.0f seconds", subjectlog[subject_i].subject, subjectlog[subject_i].studytime);
	}
	for (int subject_i = 0; subject_i < subject_count; subject_i++)
		for (int j = 0; j < subjectlog[subject_i].studytime / 5000; j++)
			mvwprintw(win, 9 + 3 * subject_i, 2 + j, "%%");

	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
}

void menu3() {
	//initscr();
	noecho();
	//wclear(win);
	// make new window
	WINDOW* win = newwin(34, 60, 1, 1);

	char menu;
	while (1) {
		menu3_screen(win);
		menu = getch();
		if (menu == '1') menu3_create(win);
		if (menu == '2') menu3_join(win);
		if (menu == '3') menu3_leave(win);
		if (menu == '4') menu3_rank(win);
		if (menu == '5') {
			break;
		}
	}
	wclear(win);
	wrefresh(win);
	delwin(win);
	return;
}

void menu3_create(WINDOW* win) {

	wclear(win);
	box(win, '|', '-');
	wrefresh(win);

	echo();

	// 사용자 이름 복사
	char user[30] = { 0 };
	strcpy(user, UID);

	// 생성할 그룹 이름 받기
	//char groupid[30] = { 0 };
	char* groupid = malloc(sizeof(char*) * 30);
	mvwprintw(win, 3, 2, "Enter GROUP ID that you want to create : ");
	mvwgetstr(win, 3, 43, groupid);
	wrefresh(win);

	// ./users 안에 생성된 그룹이 있는지 검사
	int group_flag = 0; // ./users 안에 생성된 그룹이 있는지 검사하는 flag
	DIR* users_dir = opendir("./users");
	struct dirent* ent;
	if (users_dir != NULL) {

		while ((ent = readdir(users_dir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1; // 이미 생성된 그룹 발견
			}
		}
	}
	closedir(users_dir);

	if (group_flag == 1) {
		mvwprintw(win, 5, 2, "<%s> already CREATED !!!");
		mvwprintw(win, 7, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 2;
			mvwprintw(win, 9, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}
	else // group_flag == 0, 생성된 그룹이 없을 때
	{
		// 현재 경로 ./users/no_group

		// 현재 경로 ./users로 변경하기
		if (chdir("..") != 0) {
			perror("chdir");
			return;
		}

		// ./users 안에 groupid 그룹을 생성한다
		if (mkdir(groupid, 0755) == -1) {
			perror("mkdir");
			return;
		}

		// oldpath 경로 만들기
		char old_path[50] = { 0 };
		strcpy(old_path, "./no_group/");
		strcat(old_path, user);

		// newpath 경로 만들기
		char new_path[50] = { 0 };
		strcpy(new_path, "./");
		strcat(new_path, groupid);
		strcat(new_path, "/");
		strcat(new_path, user);

		// 현재 경로 ./users
		// ./users/no_group/user 에서 ./users/groupid/user로 디렉토리 경로 바꾸기

		// 디렉토리 경로 바꾸기 실패
		if (rename(old_path, new_path) != 0) {
			perror("rename");
			return;
		}
		// 디렉토리 경로 바꾸기 성공
		else {
			int fd1, fd2;

			// 구조체에 정보 넣기
			groupinfo add;
			strcpy(add.group_name, groupid);
			add.group_users = 1;

			// 파일이 없어서 fd != -1 -> 파일 생성됨
			if ((fd1 = open("group.txt", O_RDWR | O_CREAT | O_EXCL, 0755)) > 0) {
				wrefresh(win);
				// 파일에 구조체 단위로 쓰기
				lseek(fd1, 0, SEEK_SET);
				write(fd1, &add, sizeof(groupinfo));
			}
			else { // 파일이 있어서 fd == -1임
				wrefresh(win);
				// 파일을 읽고 쓰는 권한으로 열기
				fd2 = open("group.txt", O_RDWR | O_APPEND);

				// 파일을 구조체 단위로 읽고 그룹 이름이 중복되는 것이 있는지 검사
				int isgroup = 0;
				lseek(fd2, 0, SEEK_SET);

				// 읽을 구조체 선언하기
				groupinfo rd;
				while (read(fd2, &rd, sizeof(groupinfo))) {
					// 파일에 그룹이 존재하는 경우 = 이미 존재하는 그룹 -> 그러면 안됨
					if (strcmp(rd.group_name, groupid) == 0) {
						isgroup = 1;
						break;
					}
				}
				// 파일에 그룹이 존재하지 않는 경우
				if (isgroup == 0) {
					wrefresh(win);
					lseek(fd2, 0, SEEK_END);
					write(fd2, &add, sizeof(groupinfo));
				}
			}
			// users.txt 파일에 바뀐 group 정보 replace
			int ufd = usersFd;
			int read_st = 0;
			Studyuser j_user;

			lseek(ufd, 0, SEEK_SET);
			while (read(ufd, &j_user, sizeof(Studyuser))) {
				if (strcmp(j_user.user_ID, user) == 0) {
					strcpy(j_user.group_ID, groupid); // GROUP ID is copy to j_user
					lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
					write(ufd, &j_user, sizeof(Studyuser)); // replacing
					wrefresh(win);
				}
			}
			// fd 닫아주기
			close(fd1);
			close(fd2);

			// 현재 경로 ./users
			mvwprintw(win, 5, 2, "You have CREATE and JOIN <%s> !!!", groupid);
			mvwprintw(win, 7, 2, "If you want to go BACK, press 'q' !");
			wrefresh(win);
		}
	}

	char menu;
	while ((menu = getch()) != 'q');

	free(groupid);
	wclear(win);
	wrefresh(win);
	return;
}

void menu3_join(WINDOW* win) {

	wclear(win);
	box(win, '|', '-');
	wrefresh(win);
	echo();

	// 사용자 이름 복사
	char user[30] = { 0 };
	strcpy(user, UID);

	// 들어갈 그룹 이름 받기
	char* groupid = malloc(sizeof(char*) * 30);
	mvwprintw(win, 3, 2, "Enter GROUP ID that you want to join : ");
	mvwgetstr(win, 3, 42, groupid);
	wrefresh(win);

	// 현재 경로 ==  ./users

	// ./users로 디렉토리 경로 변경
	if (chdir("..") != 0) {
		perror("chdir");
		return;
	}

	// ./users/no_group 안에 사용자가 있는지 -> 사용자가 아무 그룹에 속하지 않은 상태
	int user_flag = 0; // ./users 안에 생성된 그룹들이랑 같은지 표시하는 flag
	DIR* u_dir = opendir("./no_group");
	struct dirent* u_ent;
	if (u_dir != NULL) {
		while ((u_ent = readdir(u_dir)) != NULL) {
			if (u_ent->d_type == DT_DIR && strcmp(u_ent->d_name, user) == 0) {
				user_flag = 1; // no_group안에 사용자가 있음
			}
		}
	}
	closedir(u_dir);

	// 사용자는 그룹에 속한 상황
	if (user_flag == 0) {
		mvwprintw(win, 5, 2, "You ALREADY have a GROUP !!!");
		mvwprintw(win, 6, 2, "LEAVE group first then try again !");
		mvwprintw(win, 8, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 2;
			mvwprintw(win, 10, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}

	// ./users 안에 같은 그룹이 있는지 검사
	int group_flag = 0; // ./users 안에 생성된 그룹들이랑 같은지 표시하는 flag
	DIR* users_dir = opendir(".");
	struct dirent* ent;
	if (users_dir != NULL) {
		while ((ent = readdir(users_dir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1; // 이미 생성된 그룹 발견
			}
		}
	}
	closedir(users_dir);

	// users.txt 파일에 사용자 그룹 정보랑 일치하는지 확인
	// users.txt는 열려있음. users.txt의 fd를 ufd에 저장
	int ufd = usersFd;

	// 정보 읽을 구조체 선언
	Studyuser j_user;

	// 유저가 속한 그룹과 join하려는 그룹과 다른지 저장하는 flag
	int group_cmp = 0;

	// 커서 맨 앞으로 돌리고 구조체 단위로 정보 읽어가며 user_name 찾기
	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		// 파일에서 유저 찾기
		if (strcmp(j_user.user_ID, user) == 0) {
			// 유저의 그룹이랑 join하려는 그룹이랑 같음
			if (strcmp(j_user.group_ID, NO_GROUP) == 0) {
				group_cmp = 1;
			}
		}
	}
	// 이미 생성된 그룹 + 지금과 다른 그룹에 들어가고자 함
	if (group_flag == 1 && group_cmp == 1) {

		// oldpath 경로 만들기
		char old_path[50] = { 0 };
		strcpy(old_path, "./no_group/");
		strcat(old_path, user);

		// newpath 경로 만들기
		char new_path[50] = { 0 };
		strcpy(new_path, "./");
		strcat(new_path, groupid);
		strcat(new_path, "/");
		strcat(new_path, user);

		int fd2;
		// group.txt 파일 오픈
		fd2 = open("group.txt", O_RDWR | O_APPEND);

		// 파일을 구조체 단위로 읽고 그룹 이름이 중복되는 것이 있는지 검사
		int isgroup = 0;
		lseek(fd2, 0, SEEK_SET);

		// 읽을 구조체 선언하기
		groupinfo rd;
		int down_ten = 0;
		while (read(fd2, &rd, sizeof(groupinfo))) {
			// group.txt에 그룹 찾기
			if (strcmp(rd.group_name, groupid) == 0) {
				// 그룹 제한 10명 검사
				if (rd.group_users <= 9) {
					// 한명 더 늘리기
					rd.group_users += 1;
					lseek(fd2, -sizeof(Studyuser), SEEK_CUR);
					write(fd2, &rd, sizeof(Studyuser));
					down_ten = 1;
					break;
				}
				else {
					// 사용자에게 그룹이 없다고 말하기
					// 메뉴 돌아가기
				}

			}
		}
		close(fd2);
		// 그룹에 10이하이면
		if (down_ten == 1) {
			// 현재 경로 ./users
			// ./users/no_group/user 에서 ./users/groupid/user로 디렉토리 경로 바꾸기

			// 디렉토리 경로 바꾸기 실패
			if (rename(old_path, new_path) != 0) {
				perror("rename");
				return;
			}
			// 디렉토리 경로 바꾸기 성공
			else {

				// users.txt 파일 변경
				int ufd = usersFd;
				int read_st = 0;
				Studyuser j_user;

				lseek(ufd, 0, SEEK_SET);
				while (read(ufd, &j_user, sizeof(Studyuser))) {
					if (strcmp(j_user.user_ID, user) == 0) {
						strcpy(j_user.group_ID, groupid); // GROUP ID is copy to j_user
						lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
						write(ufd, &j_user, sizeof(Studyuser)); // replacing
						wrefresh(win);
					}
				}

				mvwprintw(win, 5, 2, "You have JOIN <%s> !!!", groupid);
				mvwprintw(win, 7, 2, "If you want to go BACK, press 'q' !");
				wrefresh(win);
			}
		}

	}
	// ./users 경로에 그룹 디렉토리가 없어 생성해야함
	if (group_flag == 0) {
		// 사용자에게 돌아가서 create 하라고 하기
		mvwprintw(win, 5, 2, "Go back and CREATE group <%s> !!!", groupid);
		mvwprintw(win, 7, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 1;
			mvwprintw(win, 9, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}
	if (group_cmp == 0) {
		// 사용자가 no_group에 속하지 않음 == 이미 그룹이 있음
		mvwprintw(win, 5, 2, "You ALREADY have a GROUP !!!");
		mvwprintw(win, 6, 2, "LEAVE group first then try again !");
		mvwprintw(win, 8, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 1;
			mvwprintw(win, 10, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}
	char menu;
	while ((menu = getch()) != 'q');

	free(groupid);
	wclear(win);
	wrefresh(win);
	return;

}
void menu3_leave(WINDOW* win) {

	wclear(win);
	box(win, '|', '-');
	wrefresh(win);

	echo();

	// 사용자 이름 복사
	char user[30] = { 0 };
	strcpy(user, UID);

	// 사용자에게 그룹 떠나는 여부 물어보기
	char yn;
	mvwprintw(win, 3, 2, "Do you want to LEAVE this group ? (y/n) : ");
	yn = mvwgetch(win, 3, 44);
	wrefresh(win);

	sleep(1);

	// 사용자가 그룹을 떠난다고 했을 때
	if (yn == 'y') {

		// users.txt 에서 사용자 찾고 groupid 가져오기
		int ufd = usersFd;
		int isgroup = 0;
		char groupid[30] = { 0 };

		Studyuser j_user;

		lseek(ufd, 0, SEEK_SET);
		while (read(ufd, &j_user, sizeof(Studyuser))) {
			if (strcmp(j_user.user_ID, user) == 0) {
				strcpy(groupid, j_user.group_ID); // GROUP ID copy
				isgroup = 1;
				break;
			}
		}
		// users.txt에 사용자가 있을 때
		if (isgroup == 1) {
			// 이미 사용자는 어떤 그룹에 속하지 않음
			if (strcmp(groupid, NO_GROUP) == 0) {
				// 이미 속하지 않았다고 사용자한테 말하기
				// 메뉴로 돌아가기
				mvwprintw(win, 5, 2, "You ALREADY don't have a GROUP !!!");
				mvwprintw(win, 6, 2, "CREATE of JOIN a group first then try again !");
				mvwprintw(win, 8, 2, "Returning to group menu...");
				wrefresh(win);

				for (int t = 1; t <= 3; t++) {
					sleep(1);
					int y = 5 * t - 1;
					mvwprintw(win, 10, y, " %d ...", 4 - t);
					wrefresh(win);
				}
				sleep(1);
				wclear(win);
				return;
			}
			// 사용자는 그룹에 속함
			else {

				// users.txt 변경 정보 적용
				Studyuser r_user;

				lseek(ufd, 0, SEEK_SET);
				while (read(ufd, &r_user, sizeof(Studyuser))) {
					if (strcmp(r_user.user_ID, user) == 0) {
						// 사용자 그룹 탈퇴 -> group id : no_group
						strcpy(r_user.group_ID, NO_GROUP); // GROUP ID copy
						lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
						// 사용자 변경 정보 덮어쓰기
						write(ufd, &r_user, sizeof(Studyuser));
					}
				}
				// 현재 작업 경로 확인
				char path[50];
				getcwd(path, 50);
				mvwprintw(win, 5, 2, "path : %s", path);
				wrefresh(win);

				sleep(3);

				// group.txt 변경하기
				int fd2;
				// group.txt 파일 오픈
				fd2 = open("group.txt", O_RDWR | O_APPEND);

				if (fd2 != -1) {
					// 파일을 구조체 단위로 읽고 그룹 이름이 중복되는 것이 있는지 검사
					int isgroup = 0;
					lseek(fd2, 0, SEEK_SET);

					// 읽을 구조체 선언하기
					groupinfo rd;
					int no_one = 0;
					while (read(fd2, &rd, sizeof(groupinfo))) {
						// group.txt에 그룹 찾기
						if (strcmp(rd.group_name, groupid) == 0) {
							// 그룹 제한 10명 검사 (남은 인원이 한명이 아닐 때)
							if (rd.group_users != 1) {
								// 한명 줄이기
								rd.group_users -= 1;
								lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
								write(fd2, &rd, sizeof(groupinfo));
								break;
							}
							// 사용자 1명 밖에 안남았을 때
							else {
								no_one = 1; // 그룹의 인원이 1명 즉 사용자 밖에 남지 않았을 때
								//group.txt에서 그 그룹 정보 지우기
								// 구조체 초기화 
								memset(&rd, 0, sizeof(groupinfo));
								lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
								write(fd2, &rd, sizeof(groupinfo));
							}

						}
					}

					// ./users/groupid/user_name 경로를 ./users/no_group/user_name으로 변경

					// newpath 경로 만들기
					char new_path[50] = { 0 };
					strcpy(new_path, "./no_group/");
					strcat(new_path, user);

					// oldpath 경로 만들기
					char old_path[50] = { 0 };
					strcpy(old_path, "./");
					strcat(old_path, groupid);
					strcat(old_path, "/");
					strcat(old_path, user);

					// 현재 경로 ./users
					// ./users/no_group/user 에서 ./users/groupid/user로 디렉토리 경로 바꾸기

					// 디렉토리 경로 바꾸기 실패
					if (rename(old_path, new_path) != 0) {
						perror("rename");
						return;
					}
					else {
						// 경로 잘 바꾸어짐
						mvwprintw(win, 5, 2, "You left the GROUP !!!");
						mvwprintw(win, 7, 2, "Returning to group menu...");
						wrefresh(win);

						for (int t = 1; t <= 3; t++) {
							sleep(1);
							int y = 5 * t - 1;
							mvwprintw(win, 9, y, " %d ...", 4 - t);
							wrefresh(win);
						}
						sleep(1);
						wclear(win);
						return;
					}

					// 실행 안되면 getcwd로 경로 확인하면서 해결하기
				}
				// group.txt 파일을 열지 못할 경우
				else {
					perror("open");
					wclear(win);
					sleep(3);
					return;
				}
				close(fd2);
			}

		}

	}
	// 사용자가 그룹을 떠나지 않는다고 했을 때
	else if (yn == 'n')
	{
		mvwprintw(win, 5, 2, "You choose to NOT LEAVE this group !!!");
		mvwprintw(win, 7, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 1;
			mvwprintw(win, 9, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}

	char menu;
	while ((menu = getch()) != 'q');


	wclear(win);
	wrefresh(win);
	return;

}
void menu3_rank(WINDOW* win) {

	wclear(win);
	box(win, '|', '-');
	wrefresh(win);

	echo();

	char groupid[30] = { 0 };
	char userid[30] = { 0 };

	mvwprintw(win, 3, 2, "Enter your USER ID : ");
	mvwgetstr(win, 3, 24, userid);

	//groupid 가지고 오기
	int ufd = usersFd;
	int read_st = 0;
	Studyuser j_user;
	int uflag = 0;
	int i = 0;

	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		if (strcmp(j_user.user_ID, userid) == 0) {
			uflag = 1; // userid is found
			strcpy(groupid, j_user.group_ID);
			//mvwprintw(win, 5, 2, "Your group id is %s", groupid);
			wrefresh(win);
			// GROUP ID is copy to j_user
		}
	}
	if (uflag == 1) { // 사용자 그룹을 얻었을 때
		//2. users 경로에서 group_name을 찾고 그 디렉토리를 연다
		int users_flag = 0;
		int group_flag = 0;
		char g_dir[60] = { 0 };

		// users/group_name 열어서 user 조회하기
		strcpy(g_dir, "../");
		strcat(g_dir, groupid);

		usertime rank[10] = { 0 };

		//char user_name[10][10];
		DIR* groupdir = opendir(g_dir); // ./user/group_name 디렉토리 열기
		if (groupdir != NULL) { // NULL값 검사
			struct dirent* ent;
			while ((ent = readdir(groupdir)) != NULL) {
				if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
					users_flag = 1;
					wrefresh(win);
					strncpy(rank[i].user_name, ent->d_name, sizeof(ent->d_name));
					i++;
				}
			}
			closedir(groupdir);
		}
		else {
			perror("Error opening group directory.\n");
			exit(1);
		}
		// 오늘 날짜 구하기
		time_t t = time(NULL);
		struct tm* now = localtime(&t);

		char dateStr[40] = { 0 };
		snprintf(dateStr, sizeof(dateStr), "%04d%02d%02d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

		// user_name 디렉토리에 들어가서 파일 존재 확인
		struct stat buf;
		char u_dir[60] = { 0 }; //users/group_name/users_name
		int today_flag = 0;

		for (int s = 0; s < i; s++) {

			strcpy(u_dir, g_dir);
			strcat(u_dir, "/");
			strcat(u_dir, rank[s].user_name);

			DIR* userDir = opendir(u_dir);
			if (u_dir != NULL) {
				struct dirent* u_entry;
				while ((u_entry = readdir(userDir)) != NULL) {
					if (strcmp(u_entry->d_name, dateStr) == 0) { // finding today record
						today_flag = 1;
					}
				}
				if (today_flag == 1) { // today file not found
					//파일열기
					int fd; // file descriptor 선언
					timelog todaytime; //timelog 구조체 todaytime 선언;

					char f_dir[100] = { 0 };// to save file path
					strcat(f_dir, u_dir);
					strcat(f_dir, "/");
					strcat(f_dir, dateStr); // making file path 

					if ((fd = open(f_dir, O_RDWR)) != -1) {
						//printf("calculating time...\n"); 
						//파일 구조체 단위로 읽기
						while (read(fd, &todaytime, sizeof(timelog))) {
							rank[s].study_time += todaytime.studytime;
						}
						close(fd);
					}
					else {
						closedir(userDir);
						break;
					}
				}
				else {
					rank[s].study_time = 0;
				}
			}
			closedir(userDir);
		}

		// rank 구조체 sorting 하기
		usertime temp;
		for (int y_index = 0; y_index < i; y_index++) {
			for (int x_index = 0; x_index < i - 1; x_index++) {
				if (rank[x_index].study_time < rank[x_index + 1].study_time) {
					temp = rank[x_index];
					rank[x_index] = rank[x_index + 1];
					rank[x_index + 1] = temp;
				}
			}
		}

		// sorting 한 결과 보여주기
		mvwprintw(win, 5, 2, "Today's <%s> ranking !", groupid);
		int x = 7;
		int y = 2;
		for (int n = 0; n < i; n++) {
			mvwprintw(win, x, y, "%d.\t%s\t%lf", n + 1, rank[n].user_name, rank[n].study_time);
			if (strcmp(rank[n].user_name, userid) == 0) {
				mvwprintw(win, x, 30, "<-");
			}
			x++;
			wrefresh(win);
		}

	}

	char menu;
	while ((menu = getch()) != 'q');

	wclear(win);
	wrefresh(win);
	return;

}
void menu3_screen(WINDOW* win) {
	int x = 3;
	int y = 7;

	box(win, '|', '-');

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
	mvwprintw(win, x + 17, y, "1. Group Create");
	mvwprintw(win, x + 18, y, "2. Group Join");
	mvwprintw(win, x + 19, y, "3. Group Leave");
	mvwprintw(win, x + 20, y, "4. Group Ranking");
	mvwprintw(win, x + 21, y, "5. Go back");
	wrefresh(win);
}

void menu4(DIR* dir_ptr)
{
	WINDOW* win = newwin(34, 60, 1, 1);

	char c;
	while (1)
	{
		menu4_screen(win);
		c = wgetch(win);
		if (c == 'q') break;
		if (c == '1') menu4_profile(win);
		if (c == '2') menu4_help(win);
		if (c == '3') menu4_deleteAccount(win, dir_ptr); // 3 누르면 계정 삭제할지 말지 선택

		if (user_dead == 1) break;
	}
	wclear(win);
	wrefresh(win);
	delwin(win);
}

void menu4_screen(WINDOW* win)
{
	box(win, '|', '-');
	mvwprintw(win, 3, 2, "Settings");
	mvwprintw(win, 6, 2, "1. My profile");
	mvwprintw(win, 8, 2, "2. help");
	mvwprintw(win, 10, 2, "3. Delete account");
	mvwprintw(win, 30, 2, "\'q\' to quit");
	wrefresh(win);
}

void menu4_profile(WINDOW* win)
{
	int ufd = usersFd;
	Studyuser s_user; // 내 정보 읽어오기
	lseek(ufd, -sizeof(Studyuser), SEEK_CUR);
	read(ufd, &s_user, sizeof(Studyuser));

	struct tm* tm_ptr;

	mvwprintw(win, 3, 2, "Settings - %s\'s profile", s_user.user_ID);
	mvwprintw(win, 6, 2, "User ID: %s", s_user.user_ID);
	mvwprintw(win, 8, 2, "Group ID: %s", s_user.group_ID);

	tm_ptr = localtime(&(s_user.signup)); // time_t-> struct tm
	mvwprintw(win, 10, 2, "                         ");
	mvwprintw(win, 11, 2, "Sign up time: %25s", asctime(tm_ptr)); // struct tm -> human_readable
	mvwprintw(win, 11, 59, "|");
	tm_ptr = localtime(&(s_user.lastlogin)); // time_t -> struct tm
	mvwprintw(win, 13, 2, "Last login time: %25s", asctime(tm_ptr)); // struct tm -> human_readable
	mvwprintw(win, 13, 59, "|");
	mvwprintw(win, 30, 2, "%s", "\'q\' to quit");

	wrefresh(win);

	char quit = '0';
	while ((quit = wgetch(win)) != 'q'); // q 누르면 나가기

	wclear(win);
	wrefresh(win);

	return;
}

void menu4_help(WINDOW* win)
{
	def_prog_mode();
	endwin();

	pid_t pid;
	if ((pid = fork()) == -1)
	{
		perror("fork");
		exit(40);
	}

	if (pid == 0) // child
	{
		execlp("view", "view", "../../README.md", NULL);
		perror("execlp");
		exit(41);
	}
	else
	{
		wait(NULL);
		reset_prog_mode();
		wrefresh(win);
	}
}

void menu4_deleteAccount(WINDOW* win, DIR* uid_dirptr)
{
	char yesno;
	curs_set(1);
	mvwprintw(win, 17, 2, "             ");
	mvwprintw(win, 13, 2, "Delete your account from StudytimeCheck? (Y/N) "); wrefresh(win);
	while (1)
	{
		yesno = wgetch(win); // 삭제하려면 y 입력
		yesno = toupper(yesno);
		if (yesno == 'Y')
		{
			mvwprintw(win, 15, 2, "Enter your ID if you really want to leave: ");
			wrefresh(win);
			mvwprintw(win, 15, 46, "          )");
			mvwprintw(win, 15, 45, "(");
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
						mvwprintw(win, 15, 46 + i, "%c", input_c);
						input[i++] = input_c;
						input[i] = '\0';
					}
				}
				if ((input_c == '\b' || input_c == 127) && i > 0) // 백스페이스로 0글자 이하로 가는 거 blocking
				{
					mvwprintw(win, 15, 46 + --i, " ");
					move(15, 1 + i);
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
						mvwprintw(win, 16, 2, "Deactivated your account. See you Again...");
						wrefresh(win);
						sleep(2);
						user_dead = 1; // 유저가 탈퇴했음.
					}
					else
					{
						mvwprintw(win, 16, 2, "Wrong zzz");
						wrefresh(win);
						sleep(2);
					}
					break;
				}
				wrefresh(win);
			}
			break;
		}
		if (yesno == 'N')
		{
			mvwprintw(win, 12, 2, "                                               ");
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

