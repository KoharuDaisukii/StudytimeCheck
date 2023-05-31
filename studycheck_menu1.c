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
#include "studytimecheck.h"
#include "varlib.h"
#include "login.h"

#define MAX 11

typedef struct usertime { //group ranking 때 필요한 구조체
	char user_name[50];
	double study_time;
}usertime;

typedef struct groupinfo {
	char group_name[50];
	int group_users;
}groupinfo;

void main_screen(WINDOW* win, int arrow_select);
void menu1();
void menu2();
void menu2_1(WINDOW* win);
void menu2_2(WINDOW* win);
void menu2_3(WINDOW* win);

void menu3();
void menu3_create(WINDOW* win);
void menu3_screen(WINDOW* win);
void menu3_join(WINDOW* win);
void menu3_leave(WINDOW* win);
void menu3_rank(WINDOW* win);

void menu4();
void menu4_1(WINDOW* win);
void menu4_2(WINDOW* win);
void menu4_3(WINDOW* win);

int user_dead = 0;
int usersFd;
char UID[11];

int main(int argc, char* argv[])
{
	login(argc, argv);
	WINDOW* main_win = initial_set();
	
	int key_input;
	int arrow_select = 1;
	while (1)
	{
		main_screen(main_win, arrow_select);
		wrefresh(main_win);
		key_input = wgetch(main_win);
		arrow_select = arrow_convert(key_input, arrow_select, 5);
		if(key_input == '1' || (key_input == '\n' && arrow_select == 1)) menu1();
		if (key_input == '2' || (key_input == '\n' && arrow_select == 2)) menu2();
		if (key_input == '3' || (key_input == '\n' && arrow_select == 3)) menu3();
		if (key_input == '4' || (key_input == '\n' && arrow_select == 4)) menu4();
		if (key_input == '5' || (key_input == '\n' && arrow_select == 5)) break;
		if (user_dead == 1) break;
	}
	unsetup();
	return 0;
}

void wcleardel(WINDOW* win)
{
	wclear(win);
	wrefresh(win);
	delwin(win);
}

int arrow_convert(int key_select, int arrow_select, int bound)
{
	if(key_select == ARROW_DOWN)
	{
		arrow_select++;
		if(arrow_select > bound)
			arrow_select = 1;
	}
	if(key_select == ARROW_UP)
	{
		arrow_select--;
		if(arrow_select < 1)
			arrow_select = bound;
	}
	return arrow_select;
}

void wprintw_quit(WINDOW* win, int line_num, int arrow_select)
{
	mvwprintw_standout(win, 35, 42, "Enter to quit", line_num, arrow_select);
}

void main_screen(WINDOW* win, int arrow_select)
{
	int x = 3;
	int  y = 0;
	
	
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
	
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	mvwprintw_standout(win, 14, 2, "1. Studytime Measuring", 1, arrow_select);
	mvwprintw_standout(win, 17, 2, "2. Display stats", 2, arrow_select);
	mvwprintw_standout(win, 20, 2, "3. Group", 3, arrow_select);
	mvwprintw_standout(win, 23, 2, "4. Settings", 4, arrow_select);
	wprintw_quit(win, 5, arrow_select);
	
	wrefresh(win);
}

void mvwprintw_standout(WINDOW* win, int y, int x, char* str, int line_num, int select)
{
	if(line_num == select)
		wstandout(win);
	mvwprintw(win, y, x, "%s", str);
	wstandend(win);
}

void wprintw_standout(WINDOW* win, char* str, int line_num, int select)
{
	if(line_num == select)
		wstandout(win);
	wprintw(win, "%s", str);
	wstandend(win);
}

void wfill(WINDOW* win, int y1, int x1, int y2, int x2, char* ch)
{
	standend();
	for(int i=y1; i<=y2; i++)
		for(int j=x1; j<=x2; j++)
			mvwprintw(win, i, j, "%s", ch);
}
/////여기서부터긁으세요.
timelog tlog; //전역에 선언해야함. 그래야 sigalrm에도 작동.
int file;//전역에 선언해야함. 그래야 sigalrm에도 작동.
int alarmcheck=0; //덮어쓰기 or 새로 쓰기 구분 위함.


void alarm_to_write(int signum){
	alarm(0);
	tlog.finish_time = time(NULL);
    //struct tm *time_info_end = localtime(&tlog.finish_time);
    tlog.studytime = difftime(tlog.finish_time, tlog.start_time);
	
	if(alarmcheck==0){ //처음 쓴다
		write(file, &tlog, sizeof(timelog));
	    alarmcheck=1;
	    alarm(30);
	}
	else if(alarmcheck==1){ //덮어쓴다.(갱신)
		lseek(file, -sizeof(timelog), SEEK_CUR); //덮어쓰기
		write(file, &tlog, sizeof(timelog));
	    alarmcheck=1;
	    alarm(30);
	}
}

void make_study_dot(WINDOW* win){
	static int position=32;
	mvwprintw(win, 15, 24, "Studying");
	wrefresh(win);
	mvwprintw(win, 15, position, ".");
	wrefresh(win);
	sleep(1);
	position++;
	if(position>34){
		mvwprintw(win, 15, 32, "   ");
		wrefresh(win);
		position=32;
	}
	
}

void menu1(){
   int usersFd;
   char filename[100];
   Studyuser studyuser;
   struct tm *time_info_start, *time_info_end;
   int key, check = 0;
   int check_point_of_before_start=0;
   char start_time_str[20];
   char finish_time_str[20];
   char studytime_str[20];
  	signal(SIGALRM, alarm_to_write);
	alarm(30);

   WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, TRUE);
	
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    	mvwprintw(win, 3, 2, "Studytime Measuring");
	mvwprintw(win, 7, 2, "Please enter the subject you want to study.");

	mvwprintw(win, 8, 33, ")");
	mvwprintw(win, 8, 2, "(");
	curs_set(1);

	char input[30] = "\0";
	int input_c;
	int i = 0;

	while (1)
	{
		input_c = wgetch(win);
		if (isalnum(input_c) || input_c == ' ')
		{
			if (i < 30)
			{
				mvwprintw(win, 8, 3 + i, "%c", input_c);
				input[i++] = input_c;
				input[i] = '\0';
			}
		}
		if ((input_c == '\b' || input_c == 263) && i > 0)
		{
			mvwprintw(win, 8, 3 + --i, " ");
			wmove(win, 8, 3 + i);
		}
		if (input_c == '\n') // 엔터 입력
		{
			curs_set(0);
			break;
		}
	}
	strcpy(tlog.subject, input);
	wfill(win, 2, 2, 10, 50, " ");
	mvwprintw(win, 16, 10, "Press spacebar to measure study time."); 
	wrefresh(win);
    	mvwprintw(win, 30, 45, "quit: q"); 
    	wrefresh(win);
	

    	while (1){
		// nodelay(win, TRUE); // 키 입력 비차단 모드 설정
        key = wgetch(win);
		if(check_point_of_before_start==0){
			check_point_of_before_start=1;
		}

    	if (key == 'q')
      	{
        	return;
      	}

      	if (key == ' ')
      	{
		if (check == 0)
        	{
            	tlog.start_time = time(NULL); // 현재 시간으로 할당
            	time_info_start = localtime(&tlog.start_time);
            	sprintf(filename, "%04d%02d%02d.txt", time_info_start->tm_year + 1900, time_info_start->tm_mon + 1, time_info_start->tm_mday);

               	chdir(UID);
               	file = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
               	if (file == -1)
               	{
               		printf("file open|create error.\n");
               	   	return;
               	}
                check = 1;

				
				
        	}

		 	
		
      	}

      	if (check == 1)
      	{
        wclear(win);
		 	
			while(1){
				box(win, '|', '-');

		 		
		 		mvwprintw(win, 30, 30, "Press spacebar to stop");

				box(win, '|', '-');

				make_study_dot(win);

                nodelay(win, TRUE); // 키 입력 비차단 모드 설정
                key = wgetch(win);
         		if (key == ' ')
         		{
            		tlog.finish_time = time(NULL);
            		time_info_end = localtime(&tlog.finish_time);
            		tlog.studytime = difftime(tlog.finish_time, tlog.start_time);
            		int hours = tlog.studytime / 3600;             // 시
            		int minutes = (int)(tlog.studytime / 60) % 60; // 분
            		int seconds_ = (int)tlog.studytime % 60;       // 초
					
					if (tlog.studytime <=5.0) { // 공부시간이 30초 이하인 경우 경고 메세지 출력
                        wclear(win);
			box(win, '|', '-');
         				wrefresh(win);
                        mvwprintw(win, 15, 17, "Study time is too short!");
                        wrefresh(win);
                        sleep(1);
                        werase(win);
                        wrefresh(win);
						continue;
                   	}
		        	else{
						lseek(file, -sizeof(timelog), SEEK_CUR);
                        write(file, &tlog, sizeof(timelog));
                        close(file);
                        break; 
                    }
		    	}
				
      		}	
			break;
			
		}
   	}
	//signal(SIGALRM,SIG_DFL);
   	chdir("..");
   	wcleardel(win);
}
/////여기까지 긁어요.

void menu2()
{
	WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, true);
	
	stats_select(win);
	
	wcleardel(win);
}

void menu2_1(WINDOW* win)
{
	daystats(win);
}

void menu2_2(WINDOW* win)
{
	weekstats(win);
}

void menu2_3(WINDOW* win)
{
	monthstats(win);
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
	wcleardel(win);
	return;
}

void menu3_create(WINDOW* win) {

	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
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
				while (read(fd2, &rd,   sizeof(groupinfo))) {
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
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
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
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
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
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
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

	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 

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

void menu4()
{
	WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, TRUE);
	
	settings_select(win);
	
	wcleardel(win);
}

void menu4_1(WINDOW* win)
{
	myprofile(win);
}

void menu4_2(WINDOW* win)
{
	display_help(win);
}

void menu4_3(WINDOW* win)
{
	delete_account(win);
}

int rmdir_r(char* path)
{
	 DIR* dir_ptr;
   	 struct dirent *file;
   	 struct stat st_buf;
   	 char filename[1024];

   	 if((dir_ptr = opendir(path)) == NULL)
		return unlink(path);
		
   	 while((file = readdir(dir_ptr)) != NULL) 
   	 {
     	 	if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        		continue;
        
		sprintf(filename, "%s/%s", path, file->d_name);

        
       	if(lstat(filename, &st_buf) == -1)
            		continue;

       	if(S_ISDIR(st_buf.st_mode)) 
       	{    	
        		if(rmdir_r(filename) == -1)
                		return -1;
        	}
        	else 
        	{
           		if(unlink(filename) == -1) 
                		return -1;
        	}
        }
    
    closedir(dir_ptr);
    
    return rmdir(path);
}

