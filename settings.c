#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include "studytimecheck.h"
#include "varlib.h"

int usersFd;

void menu4_1(WINDOW* win);
void menu4_2(WINDOW* win);
void menu4_3(WINDOW* win);

void settings_select(WINDOW* win)
{
	int c;
	int arrow_select = 1;
	while (1)
	{
		settings_screen(win, arrow_select);
		c = wgetch(win);
		arrow_select = arrow_convert(c, arrow_select, 4);
		if (c == 'q') break;
		if (c == '1' || (c == '\n' && arrow_select == 1)) menu4_1(win);
		if (c == '2' || (c == '\n' && arrow_select == 2)) menu4_2(win);
		if (c == '3' || (c == '\n' && arrow_select == 3)) menu4_3(win); 
		if (c == '4' || (c == '\n' && arrow_select == 4)) break;
		if (user_dead == 1) break;
	}
}

void settings_screen(WINDOW* win, int arrow_select)
{
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
	mvwprintw(win, 3, 2, "Settings");
	mvwprintw_standout(win, 6, 2, "1. My profile", 1, arrow_select);
	mvwprintw_standout(win, 8, 2, "2. help", 2, arrow_select);
	mvwprintw_standout(win, 10, 2, "3. Delete account", 3, arrow_select);
	mvwprintw_standout(win, 30, 2, "- Quit", 4, arrow_select);
	wrefresh(win);
}

void myprofile(WINDOW* win)
{
	int ufd = usersFd;
	Studyuser s_user; // 내 정보 읽어오기
	lseek(ufd, -sizeof(Studyuser), SEEK_CUR);
	read(ufd, &s_user, sizeof(Studyuser));

	struct tm* tm_ptr;
	wfill(win, 3, 2, 30, 59, " ");
	mvwprintw(win, 3, 2, "Settings - %s\'s profile", s_user.user_ID);
	
	int c;
	int arrow_select = 5;
	while (1)
	{
		display_profile(win, s_user, arrow_select);
		c = wgetch(win);
		arrow_select = arrow_convert(c, arrow_select, 5);

		if (c == '5' || (c == '\n' && arrow_select == 5)) break;
		if (user_dead == 1) break;
	}

	wclear(win);
	wrefresh(win);
}

void display_profile(WINDOW* win, Studyuser s_user, int arrow_select)
{
	mvwprintw_standout(win, 6, 2, "User ID: ", 1, arrow_select);
	wprintw_standout(win, s_user.user_ID, 1, arrow_select);
	mvwprintw_standout(win, 8, 2, "Group ID: ", 2, arrow_select);
	wprintw_standout(win, s_user.group_ID, 2, arrow_select);

	struct tm* tm_ptr = localtime(&(s_user.signup)); // time_t-> struct tm

	mvwprintw_standout(win, 11, 2, "Sign up time: ", 3, arrow_select); // struct tm -> human_readable
	wprintw_standout(win, asctime(tm_ptr), 3, arrow_select); // struct tm -> human_readable
	tm_ptr = localtime(&(s_user.lastlogin)); // time_t -> struct tm
	mvwprintw_standout(win, 13, 2, "Last login time: ", 4, arrow_select); // struct tm -> human_readable
	wprintw_standout(win, asctime(tm_ptr), 4, arrow_select);
	mvwprintw_standout(win, 30, 2, "ENTER to quit", 5, arrow_select);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
	wrefresh(win);
}

void display_help(WINDOW* win)
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

void delete_account(WINDOW* win)
{
	char yesno;
	curs_set(1);
	
	mvwprintw(win, 10, 2, "3. Delete account");
	mvwprintw_standout(win, 15, 2, "Delete your account from StudytimeCheck?", 1, 1);
	wprintw(win, " (Y/N) ");
	wrefresh(win);
	while (1)
	{
		yesno = wgetch(win); // 삭제하려면 y 입력
		yesno = toupper(yesno);
		if (yesno == 'Y')
		{
			mvwprintw(win, 15, 2, "Delete your account from StudytimeCheck? (Y/N) ", 1, 1);
			mvwprintw_standout(win, 17, 2, "Enter your ID if you really want to leave.", 2, 2);
			mvwprintw(win, 17, 46, "          )");
			mvwprintw(win, 17, 45, "(");
			
			wrefresh(win);
			char input[11] = "\0";
			int input_c;
			int i = 0;
			while (1)
			{
				input_c = wgetch(win);
				if (isalnum(input_c)) // 알파벳, 숫자만 허용
				{
					if (i < 10) // 10글자 이상 입력 blocking
					{
						mvwprintw(win, 17, 46 + i, "%c", input_c);
						input[i++] = input_c;
						input[i] = '\0';
					}
				}
				if ((input_c == '\b' || input_c == 263) && i > 0) // 백스페이스로 0글자 이하로 가는 거 blocking
				{
					mvwprintw(win, 17, 46 + --i, " ");
					move(17, 1 + i);
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
						if (rmdir_r(UID) == -1)
						{
							perror("rmdir_r");
							exit(50);
						}
						mvwprintw(win, 18, 2, "Deactivated your account. See you Again...");
						wrefresh(win);
						sleep(2);
						user_dead = 1; // 유저가 탈퇴했음.
					}
					else
					{
						mvwprintw(win, 18, 46, "Wrong ID!!");
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
			break;
		}
		wrefresh(win);
	}
	wclear(win);
	wrefresh(win);
	noecho(); cbreak(); curs_set(0);
	return;
}
