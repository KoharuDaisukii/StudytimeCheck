#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <curses.h>
#include <time.h>

typedef struct timelog{
	char subject[30];
	time_t start_time;//공부 시작 시간
	time_t finish_time;//공부 종료 시간
	double studytime;// 공부 시간
}timelog;


/*typedef struct tm {
   int tm_sec;         // 초,  range 0 to 59
   int tm_min;         // 분, range 0 to 59
   int tm_hour;        // 시간, range 0 to 23
   int tm_mday;        // 일, range 1 to 31
   int tm_mon;         // 월, range 0 to 11
   int tm_year;        // 1900년 부터의 년
   int tm_wday;        // 요일, range 일(0) to 토(6)
   int tm_yday;        // 1년 중 경과 일, range 0 to 365
   int tm_isdst;       // 섬머타임 실시 여부 (양수, 0, 음수)
}tm;*/

int check=0;

int main(int argc,char* argv[]){
   char filename[100];
   timelog log;
   timelog next_daylog; // 하루 넘어가는 ex)11:00->00:12로 가면 다음 날에 저장해서 하기위함.
   int file;
   struct tm *time_info_start, *time_info_end;
   int key, check = 0;
   char start_time_str[20];
   char finish_time_str[20];
   char studytime_str[20];


   initscr();
   addstr("Please enter the subject you want to study: ");
   refresh();
   getstr(log.subject); // 과목 받기
  
   addstr("Press spacebar to measure study time.");
   refresh();
   move(LINES - 1, COLS - strlen("quit: q"));
   addstr("quit: q");
   refresh();
   clear();


	while(1){ 
      
        key = getch();
      	if (key == ' ') {
         	if (check == 0) {
            		log.start_time = time(NULL); // 현재 시간으로 할당
            		time_info_start = localtime(&log.start_time);
                  sprintf(filename,"%04d%02d%02d.txt",time_info_start->tm_year+1900,time_info_start->tm_mon+1,time_info_start->tm_mday);
                  move(LINES/2, COLS/2);
                  addstr("studying...");
                  refresh();
                  move(LINES - 1, COLS - strlen("Press spacebar to stop"));
                  addstr("Press spacebar to stop");
                  refresh();

                  file = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                  if (file == -1) {
                     printf("file open|create error.\n");
                     return 1;
                  }

                  
                  //sprintf(start_time_str, " %02d:%02d:%02d", time_info_start->tm_hour,time_info_start->tm_min,time_info_start->tm_sec);//문자열로 변환
                  //printf("%d-%d-%d %d:%d:%d\n",time_info_start->tm_year+1900,time_info_start->tm_mon+1,time_info_start->tm_mday,time_info_start->tm_hour,time_info_start->tm_min,time_info_start->tm_sec);
            		check = 1;
         	}
				
      	}

         if (check == 1) {
           // printf("Press spacebar twice to end study time measurement.\n");
            key = getch();
            if (key == ' ') {			
            		log.finish_time = time(NULL);
         			time_info_end = localtime(&log.finish_time);

                  //strcpy(start_time_str,ctime(&log.start_time));
                  //strcpy(finish_time_str,ctime(&log.finish_time));
                  log.studytime=difftime(log.finish_time,log.start_time);
                  int hours=log.studytime/3600; //시
                  int minutes=(int)(log.studytime/60)%60; //분
                  int seconds_=(int)log.studytime%60;//초
                  //윗 세줄은 00:00:00의 형태로 나타낼때 사용
                  //sprintf(studytime_str, "%f", log.studytime);

                  //sprintf(finish_time_str, " %02d:%02d:%02d ", time_info_end->tm_hour,time_info_end->tm_min,time_info_end->tm_sec);//문자열로 변환
                  write(file, &log, sizeof(timelog));
                  //write(file,log.subject,strlen(log.subject));
                  //write(file, start_time_str, strlen(start_time_str));
                  //write(file, finish_time_str, strlen(finish_time_str));
                  //write(file, log.studytime, strlen(log.studytime));
                  close(file);
                  //printf("%d-%d-%d %d:%d:%d\n",time_info_end->tm_year+1900,time_info_end->tm_mon+1,time_info_end->tm_mday,time_info_end->tm_hour,time_info_end->tm_min,time_info_end->tm_sec);
          			break;
   			}
         }
	}


  //double seconds=difftime(log.finish_time,log.start_time);
  //int hours=seconds/3600;
  //int minutes=(int)(seconds/60)%60;
  //int seconds_=(int)seconds%60;
  endwin();
   //printf("%d:%d:%d\n",hours,minutes,seconds_);
   return 0;
}	


