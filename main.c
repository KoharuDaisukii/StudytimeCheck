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

void menu1();

void main_screen(WINDOW* win, int arrow_select);
void menu2();
void menu2_1(WINDOW* win);
void menu2_2(WINDOW* win);
void menu2_3(WINDOW* win);

void menu3();
void menu3_create(WINDOW* win);
void menu3_screen(WINDOW* win, int arrow_select);
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
		if (key_input == '1' || (key_input == '\n' && arrow_select == 1)) menu1();
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
		sleep(1);
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
	
	mvwprintw(win, 16, 2, "Press spacebar to measure study time."); 
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
         		wrefresh(win);
		 		
		 		mvwprintw(win,30, 30, "Press spacebar to stop");
		 		wrefresh(win);

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

void menu2()
{
	WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, TRUE);
	
	stats_select(win);
	
	wcleardel(win);
}

void menu3() 
{
	WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, TRUE);
	
	group_menuselect(win);
	
	wcleardel(win);
}

void menu4()
{
	WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, TRUE);
	
	settings_select(win);
	
	wcleardel(win);
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

