#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strlen
#include <ctype.h> // toupper
#include <sys/stat.h> // mkdir
#include <sys/types.h> // mkdir, lseek
#include <fcntl.h> // open
#include <dirent.h> // struct dirent
#include <unistd.h> // chdir, write, lseek
#include <curses.h>
#include <time.h> // time, struct tm

#define USER_INFO_DIR "users"
#define USER_INFO_FILE "user_info.txt"
#define NO_GROUP "default"

typedef struct Studyuser
{
	char user_ID[11];
	char group_ID[11];
	// 가입 일자
	// 최종 접속 시간
	// 또 뭐 넣지
} Studyuser;

typedef struct timelog // 공부 시간 기록을 저장하는 구조체
{	
	// Studyuser info; 
	struct tm* start_time; // 공부 시작 시간
	struct tm* finish_time; // 공부 종료 시간
} timelog;

/* struct tm {
   int tm_sec;         // 초,  range 0 to 59      
   int tm_min;         // 분, range 0 to 59             
   int tm_hour;        // 시간, range 0 to 23  
   int tm_mday;        // 일, range 1 to 31  
   int tm_mon;         // 월, range 0 to 11     
   int tm_year;        // 1900년 부터의 년                
   int tm_wday;        // 요일, range 일(0) to 토(6) 
   int tm_yday;        // 1년 중 경과 일, range 0 to 365 
   int tm_isdst;       // 섬머타임 실시 여부 (양수, 0, 음수)              
}; */

DIR* login(char* UID); // login 성공: UID directory 포인터 return


int main(int argc, char* argv[])
{
	if(argc == 1) // 아이디 입력 안 함
	{	
		fprintf(stderr, "아이디를 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(1);
	}
	else if(strlen(argv[1]) >= 11) // 아이디 10글자 이하로 입력 안 함
	{
		fprintf(stderr, "아이디는 10글자 이하로 입력해주세요.\n");
		exit(1);
	}
	else if(argc >= 3) // 쓸데없는 것 입력함
	{
		fprintf(stderr, "ID만 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(1);
	}
	
	char UID[11]; strcpy(UID, argv[1]); // 
	DIR* UID_dirptr = login(UID); // 로그인해서 UID 이름의 폴더를 엶
	
	initscr();
	noecho();
	move(1, 1);
	printw("Welcome! %s!", UID);
	move(3, 1);
	printw("1. Studytime Measuring");
	move(4, 1);
	printw("2. Display Stats");
	move(5, 1);
	printw("3. Group");
	move(6, 1);
	printw("4. Settings");
	move(7, 1);
	printw("5. Exit");
	char menu;
	while(1)
	{
		menu = getch();
		// if(menu == '1') menu1();
		// if(menu == '2') menu2();
		// if(menu == '3') menu3();
		// if(menu == '4') menu4();
		if(menu == '5') break;
	}
	endwin();
	closedir(UID_dirptr); // 로그아웃
	printf("%s님, Bye Bye!\n", UID);
	return 0;
}

DIR* login(char* UID)
{
	DIR *dir_ptr;
	
	if(chdir(USER_INFO_DIR) == -1) // 유저 정보가 담긴 디렉토리로 이동
	{
		perror(USER_INFO_DIR);
		exit(1);
	}
	
	int fd, user_exist = 0; //
	if((fd = open(USER_INFO_FILE, O_RDWR)) == -1) // 유저들의 정보를 담아놓은 파일을 Open
	{
		perror("왜 안 열려");
		exit(1);
	}
	
	Studyuser s_userptr;
	while(read(fd, &s_userptr, sizeof(Studyuser)) >= sizeof(Studyuser))
		if(strcmp(s_userptr.user_ID, UID) == 0) // 해당하는 UID의 유저가 있는지 탐색
			user_exist = 1; // 있네
	
	// 한글 ID 입력하면 컷하는 기능도 필요할 듯
	if((dir_ptr = opendir(UID)) == NULL || user_exist == 0) // UID 폴더 존재하는 지 확인, 그룹 생각 안 하고 일단 함
	{
		printf("ID가 존재하지 않습니다. 해당 ID로 가입하시겠습니까? (Y/N)\n");
		char yesno = toupper(getchar());
		while(yesno != 'Y' && yesno != 'N') // y도 아니고 n도 아니면 반복
		{
			printf("Y 또는 N만 입력해라: ");
			yesno = toupper(getchar()); 
		}

		if(yesno == 'Y') // Y 입력
		{
			if(mkdir(UID, 0777) == -1) // UID 이름의 디렉토리 만듦 = 가입
			{
				perror("mkdir error");
				exit(1);
			}
			if((dir_ptr = opendir(UID)) == NULL) // UID 디렉토리 열기
			{
				perror("안 열려요");
				exit(1);
			}
			
			// user_info 파일에 유저 정보 추가
			Studyuser newuser;
			strcpy(newuser.user_ID, UID);
			strcpy(newuser.group_ID, NO_GROUP);
			lseek(fd, 0, SEEK_END);
			write(fd, &newuser, sizeof(Studyuser));
			close(fd);
			// 추가 완료
			
			printf("가입 완료되었습니다. %s님 환영합니다.\n", UID);
			sleep(2); // 로딩하는 척
		}
		else
		{
			printf("\'가입하지 않기\'를 선택하셨습니다. 프로그램이 종료됩니다.\n");
			sleep(2);
			exit(1);
		}
	}
	
	return dir_ptr;
}
