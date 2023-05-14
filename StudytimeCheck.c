#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strlen
#include <ctype.h> // toupper
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

DIR* login(char* UID); // login 성공: UID directory 포인터 return

void main_screen(char* UID);
void menu4();
void menu5();

int usersFd; // USERS_INFO_FILE file descriptor

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
		exit(2);
	}
	else if(argc >= 3) // 쓸데없는 것 입력함
	{
		fprintf(stderr, "ID만 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(3);
	}
	
	char UID[11]; strcpy(UID, argv[1]); // 
	DIR* UID_dirptr = login(UID); // 로그인해서 UID 이름의 폴더를 엶
	
	initscr();
	noecho();
	
	char menu;
	while(1)
	{
		main_screen(UID);
		menu = getch();
		// if(menu == '1') menu1();
		// if(menu == '2') menu2();
		// if(menu == '3') menu3();
		if(menu == '4') menu4();
		// if(menu == '5') menu5();
		if(menu == '6') break;
	}
	
	// delwin(main_screen);
	endwin();
	closedir(UID_dirptr); // 로그아웃
	close(usersFd);
	printf("%s님, Bye Bye!\n", UID);
	return 0;
}

DIR* login(char* UID)
{
	DIR *dir_ptr;

	if(chdir(USERS_INFO_DIR) == -1) // 유저 정보가 담긴 디렉토리로 이동
	{
		if(mkdir(USERS_INFO_DIR, 0755) == -1 || chdir(USERS_INFO_DIR) == -1) // 없으면 만듦
		{
			perror(USERS_INFO_DIR);
			exit(4);
		}

	}

	int tempfd1, tempfd2;
	int user_exist = 0; //
	if((tempfd1 = open(USERS_INFO_FILE, O_RDWR | O_CREAT | O_EXCL, 0777)) == -1) // 파일 만들기 시도
	{
		if((tempfd2 = open(USERS_INFO_FILE, O_RDWR)) == -1) // 이미 있네
		{
			perror("open fd2");
			exit(9);
		}
		if((usersFd = dup(tempfd2)) == -1) // usersFd에 tempfd2 복제
		{
			perror("dup2-1");
			exit(9);
		}
		close(tempfd2);
	}
	else // 파일 없었을 때(이 시점에서는 존재함)
	{
		if((usersFd = dup(tempfd1)) == -1) // usersFd에 tempfd2 복제
		{
			perror("dup2-2");
			exit(9);
		}
		close(tempfd1);
	}

	Studyuser s_user;
	while(read(usersFd, &s_user, sizeof(Studyuser)) >= sizeof(Studyuser))	
		if(strcmp(s_user.user_ID, UID) == 0) // 해당하는 UID의 유저가 있는지 탐색
		{	
			user_exist = 1; // 있네
			break; // file pointer는 찾은 유저의 기록 바로 다음을 가리킴
		}
			
	// 한글 ID 입력하면 컷하는 기능도 필요할 듯
	if((dir_ptr = opendir(UID)) == NULL || user_exist == 0) // UID 폴더 존재하는 지 확인, 그룹 생각 안 하고 일단 함
	{
		printf("ID가 존재하지 않습니다. 해당 ID로 가입하시겠습니까? (Y/N): ");
		char yesno = toupper(getchar());
		while(yesno != 'Y' && yesno != 'N') // y도 아니고 n도 아니면 반복
		{
			printf("Y 또는 N만 입력해라: ");
			yesno = toupper(getchar()); 
		}

		if(yesno == 'Y') // Y 입력
		{
			if(mkdir(UID, 0755) == -1) // UID 이름의 디렉토리 만듦 = 가입
			{
				perror("mkdir error");
				exit(6);
			}
			if((dir_ptr = opendir(UID)) == NULL) // UID 디렉토리 열기
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

void main_screen(char* UID)
{
	move(1, 1);
	printw("Welcome! %s!", UID);
	move(3, 1);
	printw("1. Studytime Measuring");
	move(4, 1);
	printw("2. Display Stats");
	move(5, 1);
	printw("3. Group");
	move(6, 1);
	printw("4. My profile");
	move(7, 1);
	printw("5. Settings");
	move(8, 1);
	printw("6. Exit");
	refresh();
}

void menu4()
{
	WINDOW *win = newwin(20, 50, 1, 1);
	box(win, '|', '+');
	
	Studyuser s_user;
	lseek(usersFd, -sizeof(Studyuser), SEEK_CUR);
	read(usersFd, &s_user, sizeof(Studyuser));
	
	struct tm *tm_ptr;

	mvwprintw(win, 3, 2, "%s\'s profile", s_user.user_ID);
	mvwprintw(win, 5, 2, "User ID: %s", s_user.user_ID);
	mvwprintw(win, 6, 2, "Group ID: %s", s_user.group_ID);
	
	if((tm_ptr = localtime(&(s_user.signup))) == NULL)
	{
		perror("localtime");
		return;
	}
	mvwprintw(win, 8, 2, "Sign up time: %25s", asctime(tm_ptr));
	if((tm_ptr = localtime(&(s_user.lastlogin))) == NULL)
	{
		perror("localtime");
		return;
	}
	mvwprintw(win, 9, 2, "Last login time: %25s", asctime(tm_ptr));
	mvwprintw(win, 17, 2, "%s", "\'q\' to quit");
	
	wrefresh(win);
	
	char quit = '0';
	while((quit = getch()) != 'q');
	clear();
	wrefresh(win);
	delwin(win);
	
	return;
}

void menu5()
{
	WINDOW *win = newwin(20, 50, 1, 1);
	box(win, '|', '+');
}
