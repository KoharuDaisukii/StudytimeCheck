#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // strlen
#include <ctype.h>     // toupper, isalpha, isdigit
#include <sys/stat.h>  // mkdir
#include <sys/types.h> // mkdir, lseek
#include <fcntl.h>     // open
#include <dirent.h>    // struct dirent
#include <unistd.h>    // mkdir, chdir, write, lseek, dup(1,2)
#include <curses.h>
#include <time.h> // time, struct tm
#define MAX 11

typedef struct Studyuser
{
	char user_ID[11];
	char group_ID[11];
	time_t signup;// 가입 일자
	time_t lastlogin;// 최종 접속 시간
	// 또 뭐 넣지
} Studyuser;

typedef struct timelog
{
   char subject[30];
   time_t start_time;  // 공부 시작 시간
   time_t finish_time; // 공부 종료 시간
   double studytime;   // 공부 시간
} timelog;

typedef struct usertime
{
   char user_name[10];
   double study_time;
} usertime;

int check = 0;
char UID[11] = "CHARIE"; // 유저네임 임의 설정
int main(int argc, char *argv[])
{
   int usersFd;
   char filename[100];
   Studyuser studyuser;
   timelog log;
   timelog next_daylog; // 하루 넘어가는 ex)11:00->00:12로 가면 다음 날에 저장해서 하기위함.
   int file;
   struct tm *time_info_start, *time_info_end;
   int key, check = 0;
   char start_time_str[20];
   char finish_time_str[20];
   char studytime_str[20];
   char *groupid = malloc(sizeof(char *) * MAX);

   initscr();
   addstr("Please enter the subject you want to study: ");
   refresh();
   getstr(log.subject); // 과목 받기
   clear();

   addstr("Press spacebar to measure study time.");
   move(LINES - 1, COLS - strlen("quit: q"));
   addstr("quit: q");
   refresh();

   while (1)
   {
      key = getch();
      if (key == 'q')
      {
         endwin();
         return 0;
      }

      if (key == ' ')
      {
         clear();
         move(LINES / 2, COLS / 2);
         addstr("studying...");
         refresh();
         move(LINES - 1, COLS - strlen("Press spacebar to stop"));
         addstr("Press spacebar to stop");
         refresh();

         if (check == 0)
         {
            log.start_time = time(NULL); // 현재 시간으로 할당
            time_info_start = localtime(&log.start_time);
            sprintf(filename, "%04d%02d%02d.txt", time_info_start->tm_year + 1900, time_info_start->tm_mon + 1, time_info_start->tm_mday);

               chdir(UID);
               file = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
               if (file == -1)
               {
                  printf("file open|create error.\n");
                  return 1;
               }
                check = 1;
            
         }
      }

      if (check == 1)
      {
        
         key = getch();
         if (key == ' ')
         {
            log.finish_time = time(NULL);
            time_info_end = localtime(&log.finish_time);
            log.studytime = difftime(log.finish_time, log.start_time);
            int hours = log.studytime / 3600;             // 시
            int minutes = (int)(log.studytime / 60) % 60; // 분
            int seconds_ = (int)log.studytime % 60;       // 초

            write(file, &log, sizeof(timelog));

            close(file);

            break;
         }
      }
   }
   chdir("..");
   endwin();
   return 0;
}
