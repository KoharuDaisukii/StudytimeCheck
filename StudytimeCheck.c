#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strlen
#include <ctype.h> // toupper
#include <sys/stat.h> // mkdir
#include <sys/types.h> // mkdir
#include <dirent.h> // struct dirent
#include <unistd.h> // chdir
#include <curses.h>

#define USER_INFO_DIR "user_info"

DIR* login(char* wantID); // login 성공: UID directory 포인터 return


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
	printw("Welcome! %s", UID);
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
	printf("%s님 bye bye\n", UID);
	return 0;
}

DIR* login(char* UID)
{
	DIR *dir_ptr;
	struct dirent *direntp;
	
	if(chdir(USER_INFO_DIR) == -1) // 유저 정보가 담긴 디렉토리로 이동
	{
		perror(USER_INFO_DIR);
		exit(1);
	}
	// 한글 ID 입력하면 컷하는 기능도 필요할 듯
	if((dir_ptr = opendir(UID)) == NULL) // UID 폴더 존재하는 지 확인
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
			printf("가입 완료되었습니다. %s님 환영합니다.\n", UID);
			sleep(3); // 로딩하는 척
		}
		else
		{
			printf("\'가입하지 않기\'를 선택하셨습니다. 프로그램이 종료됩니다.\n");
			sleep(3);
			exit(1);
		}
	}
	
	return dir_ptr;
}
