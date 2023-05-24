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

#define MAX 11

typedef struct timelog{
	char subject[30];
	time_t start_time;//공부 시작 시간
	time_t finish_time;//공부 종료 시간
	double studytime;// 공부 시간
}timelog;

typedef struct usertime {
	char user_name[10];
	double study_time;
}usertime;

int check=0;
char UID[11]="CHARIE";//유저네임 임의 설정
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
   char* groupid = malloc(sizeof(char*) * MAX);

   initscr();
   addstr("Please enter the subject you want to study: ");
   refresh();
   getstr(log.subject); // 과목 받기
   clear();

   addstr("Press spacebar to measure study time.");
   move(LINES - 1, COLS - strlen("quit: q"));
   addstr("quit: q");
   refresh();  
   


	while(1){ 
        key = getch();
        if(key=='q'){
         endwin();
         return 0;
        }

      	if (key == ' ') {
            clear();
            move(LINES/2, COLS/2);
            addstr("studying...");
            refresh();
            move(LINES - 1, COLS - strlen("Press spacebar to stop"));
            addstr("Press spacebar to stop");
            refresh();

         	if (check == 0) {
            		log.start_time = time(NULL); // 현재 시간으로 할당
            		time_info_start = localtime(&log.start_time);
                  sprintf(filename,"%04d%02d%02d.txt",time_info_start->tm_year+1900,time_info_start->tm_mon+1,time_info_start->tm_mday);
                  
                  int users_flag = 0;
	               int group_flag = 0;
	               char g_dir[30] = { 0 };

                  mkdir("./users",0644); //없니 만들렴^^
	               //users 디렉토리의 해당 group_name의 디렉토리 있는지 여부
	               DIR* dir_info = opendir("./users"); // ./user 디렉토리 열기
	                  if (dir_info != NULL) { // NULL값 검사
		               struct dirent* ent;
		               while ((ent = readdir(dir_info)) != NULL) {
			               if (/*ent->d_type == DT_DIR &&*/ strcmp(ent->d_name, groupid) == 0) {
				                 group_flag = 1;
				                 //printf("find group\n");
		                  }
		               }
		                closedir(dir_info); 
	                  }
	                  else {
		                  perror("Error opening directory.1\n");
		                  exit(1);
	                  }

	// users/group_name 열어서 user 조회하기
	strcat(g_dir, "./users/");
	strcat(g_dir, groupid);
	
   struct stat buf;
	char u_dir[40] = { 0 }; //users/group_name/users_name
	int today_flag = 0;

		strcpy(u_dir, g_dir);
		strcat(u_dir, "/");
		strcat(u_dir, UID);// 유저네임
      mkdir(u_dir,0644); //없니? 없으면 만들렴..^^ 있으면 넘어가구..
		DIR* userDir = opendir(u_dir);
		if (u_dir != NULL) {
			struct dirent* u_entry;
			
			char f_dir[30]; // to save file path
			strcat(f_dir, u_dir);
			strcat(f_dir, "/");
			strcat(f_dir, filename); // making file path 

                  file = open(f_dir, O_WRONLY | O_CREAT | O_APPEND, 0644);
                  if (file == -1) {
                     printf("file open|create error.\n");
                     return 1;
                  }

                  //sprintf(start_time_str, " %02d:%02d:%02d", time_info_start->tm_hour,time_info_start->tm_min,time_info_start->tm_sec);//문자열로 변환
                  //printf("%d-%d-%d %d:%d:%d\n",time_info_start->tm_year+1900,time_info_start->tm_mon+1,time_info_start->tm_mday,time_info_start->tm_hour,time_info_start->tm_min,time_info_start->tm_sec);
            		check = 1;
      }
      else{
		   perror("Error opening directory.2\n");
		   exit(1);
      }  
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
      
                  write(file, &log, sizeof(timelog));
                
                  close(file);
                  
          			break;
   			}
         }
	}

  endwin();
   return 0;
}	


