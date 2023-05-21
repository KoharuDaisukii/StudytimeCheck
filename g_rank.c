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

int main(){
		
	char* groupid = malloc(sizeof(char*) * MAX);

	printf("groupid : ");
	scanf("%s", groupid);

	//2. users 경로에서 group_name을 찾고 그 디렉토리를 연다
	int users_flag = 0;
	int group_flag = 0;
	char g_dir[30] = { 0 };

	//users 디렉토리의 해당 group_name의 디렉토리 있는지 여부
	DIR* dir_info = opendir("./users"); // ./user 디렉토리 열기
	if (dir_info != NULL) { // NULL값 검사
		struct dirent* ent;
		while ((ent = readdir(dir_info)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1;
				printf("find group\n");
			}
		}
		closedir(dir_info); 
	}
	else {
		perror("Error opening directory.\n");
		exit(1);
	}

	// users/group_name 열어서 user 조회하기
	strcat(g_dir, "./users/");
	strcat(g_dir, groupid);

	typedef struct usertime {
		char user_name[10];
		double user_time;
	}usertime;
	
	usertime rank[10];

	int i = 0;
	char user_name[10][10];
	DIR* groupdir = opendir(g_dir); // ./user/group_name 디렉토리 열기
	if (groupdir != NULL) { // NULL값 검사
		struct dirent* ent;
		while ((ent = readdir(groupdir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name,".") !=0 && strcmp(ent->d_name,"..") != 0) {
				users_flag = 1;
				strcpy(rank[i].user_name, ent->d_name);
				i++;
			}
		}
		closedir(groupdir); 
	}
	else {
		perror("Error opening group directory.\n");
		exit(1);
	}
	printf("%d\n", i);
	
	// 오늘 날짜 구하기
	time_t t = time(NULL); 
	struct tm* now = localtime(&t); 

	char dateStr[20];
	snprintf(dateStr, sizeof(dateStr), "%04d%02d%02d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

	printf("Today's date: %s\n", dateStr);

	// user_name 디렉토리에 들어가서 파일 존재 확인
	struct stat buf;
	char u_dir[40] = { 0 }; //users/group_name/users_name
	int today_flag = 0;
	
	for (int s = 0; s < i; s++) {
		strcat(u_dir, g_dir);
		strcat(u_dir, "/");
		strcat(u_dir, rank[i].user_name);
		printf("user dir : %s\n", u_dir);

		DIR* userDir = opendir(u_dir);
		if (u_dir != NULL) {
			struct dirent* u_entry;
			while ((u_entry = readdir(userDir)) != NULL) {
				if (u_entry->d_type == DT_REG && strcmp(u_entry->d_name, dateStr) == 0) { // 오늘 파일 존재 확인
					today_file = 1;
				}
				else {
					printf("There is no records about today !!");
					exit(1);
				}
			}
			//파일열기
			int fd; // file descriptor 선언
			timelog todaytime; //timelog 구조체 todaytime 선언;

			if ((fd = open(dateStr, O_RDONLY)) != -1) {
				//파일 구조체 단위로 읽기
				while (read(fd, &todaytime, sizeof(timelog)) {
					rank[i].user_time += todaytime.studytime;
				}
				fclose(today_file); 
			}
			else {
				printf("Error opening %s file.\n", dateStr);
				closedir(dir); 
				return 1; 
			}
		}
		closedir(userDir);
	}
	// rank 구조체 sorting 하기
	// sorting 한 결과 보여주기

	

	free(groupid);
	return 0;
}
