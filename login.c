#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h> 
#include <curses.h>
#include <time.h>
#include "studytimecheck.h"
#include "login.h"

int usersFd;
char UID[11];

void login(int argc, char* argv[])
{
	exist_IDerror(argc, argv);
	set_forFirstRun();
	strcpy(UID, argv[1]); 	
	
	if ((usersFd = open(USERS_INFO_FILE, O_RDWR)) == -1)
	{
		perror("open fd2");
		exit(9);
	}
	
	int user_exist = 0;
	Studyuser s_user;
	while (read(usersFd, &s_user, sizeof(Studyuser)) >= sizeof(Studyuser))
	{
		if (strcmp(s_user.user_ID, UID) == 0) // 해당하는 UID의 유저가 있는지 탐색
		{
			user_exist = 1; // 있네
			break; // file pointer는 찾은 유저의 기록 바로 다음을 가리킴
		}
	}
	if (user_exist && chdir(s_user.group_ID) == -1) // 유저 정보가 담긴 디렉토리로 이동
	{
		chdir(NO_GROUP);
	}
	if (user_exist == 0)
	{
		if (chdir(NO_GROUP) == -1)
		{
			perror("chdir to NO_GROUP");
			exit(4);
		}
	}

	DIR* dir_ptr;
	if ((dir_ptr = opendir(UID)) == NULL || user_exist == 0) 
	{
		printf("ID가 존재하지 않습니다. 해당 ID로 가입하시겠습니까? (Y/N): ");
		char yesno = toupper(getchar());
		while (yesno != 'Y' && yesno != 'N') // y도 아니고 n도 아니면 반복
		{
			printf("Y 또는 N만 입력해라: ");
			yesno = toupper(getchar());
		}

		if (yesno == 'Y') // Y 입력
		{
			if (mkdir(UID, 0755) == -1) // UID 이름의 디렉토리 만듦 = 가입
			{
				perror("mkdir error");
				exit(6);
			}
			if ((dir_ptr = opendir(UID)) == NULL) // UID 디렉토리 열기
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
			return;
		}
		else
		{
			printf("\'가입하지 않기\'를 선택하셨습니다. 프로그램이 종료됩니다.\n");
			unsetup();
			sleep(2);
			exit(0);
		}
	}

	// 로그인 시간 갱신
	s_user.lastlogin = time(NULL);
	lseek(usersFd, -sizeof(Studyuser), SEEK_CUR);
	write(usersFd, &s_user, sizeof(Studyuser));
	// 추가 완료
	closedir(dir_ptr);
	return;
}

void exist_IDerror(int argc, char* argv[])
{
	int ID_result = ID_check(argc, argv);
	if (ID_result == 1) // 아이디 입력 안 함
	{
		fprintf(stderr, "아이디를 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(1);
	}
	else if (ID_result == 2) // 아이디 10글자 이하로 입력 안 함
	{
		fprintf(stderr, "아이디는 10글자 이하로 입력해주세요.\n");
		exit(2);
	}
	else if (ID_result == 3) // 쓸데없는 것 입력함
	{
		fprintf(stderr, "실행파일 이름과 아이디만 입력해주세요. Usage: ./StudytimeCheck YOUR_ID\n");
		exit(3);
	}
	else if(ID_result == 4)
	{
		fprintf(stderr, "아이디는 알파벳이나 숫자만 입력 가능합니다.\n");
		exit(4);
	}
}

int ID_check(int argc, char* argv[])
{
	if (argc == 1) // 아이디 입력 안 함
	{
		return 1;
	}
	else if (strlen(argv[1]) >= 11) // 아이디 10글자 이하로 입력 안 함
	{
		return 2;
	}
	else if (argc >= 3) // 쓸데없는 것 입력함
	{
		return 3;
	}
	
	for(int i=0; i<strlen(argv[1]); i++)
	{
		if(!isalnum(argv[1][i])) // 알파벳이나 숫자가 아니면
		{
			return 4;
		}
	}
	return 0;
}

WINDOW* initial_set()
{
	initscr();
	noecho();
	curs_set(0);
	WINDOW* win = newwin(38, 60, 1, 1);
	keypad(win, true);
	
	return win;
}

void set_forFirstRun()
{
	mkdir(USERS_INFO_DIR, 0755);
	chdir(USERS_INFO_DIR);
	int fd1 = open(USERS_INFO_FILE, O_RDWR | O_CREAT | O_EXCL, 0777);
	if(fd1 != -1)
		close(fd1);
	int fd2 = open("group.txt", O_RDWR | O_CREAT | O_EXCL, 0777);
	if(fd2 != -1)
		close(fd2);
	mkdir(NO_GROUP, 0777);
}

void unsetup()
{
	echo();
	curs_set(1);
	keypad(stdscr, false);
	endwin();
	close(usersFd);
	printf("%s님, Bye Bye!\n", UID);
}
