#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <fcntl.h> 
#include <dirent.h> 
#include <unistd.h> 
#include <curses.h>
#include <time.h> // time, struct tm
#include "studytimecheck.h"
#include "varlib.h"
#include "login.h"

typedef struct usertime { //group ranking 때 필요한 구조체
	char user_name[11];
	double study_time;
}usertime;

typedef struct groupinfo {
	char group_name[11];
	int group_users;
}groupinfo;

char UID[11];
int usersFd;


int menu3_screen_join(WINDOW* win, int key, int arrow_select);

void group_menuselect(WINDOW* win)
{
	int menu;
	int arrow_select = 1;
	while (1) {
		group_mainscreen(win, arrow_select);
		menu = wgetch(win);
		arrow_select = arrow_convert(menu, arrow_select, 5);
		if (menu == '1' || (menu == '\n' && arrow_select == 1)) group_join(win);
		if (menu == '2' || (menu == '\n' && arrow_select == 2)) group_create(win);
		if (menu == '3' || (menu == '\n' && arrow_select == 3)) group_rank(win);
		if (menu == '4' || (menu == '\n' && arrow_select == 4)) group_leave(win);
		if (menu == '5' || (menu == '\n' && arrow_select == 5)) break;
	}
}

void group_mainscreen(WINDOW* win, int arrow_select)
{
	int x = 3;
	int y = 2;

	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

	mvwprintw(win, x, y, "Group");

	mvwprintw_standout(win, x + 4, y, "1. Join a new group", 1, arrow_select);
	mvwprintw_standout(win, x + 7, y, "2. Create a new group", 2, arrow_select);
	mvwprintw_standout(win, x + 10, y, "3. Group Ranking", 3, arrow_select);
	mvwprintw_standout(win, x + 13, y, "4. Leave the group", 4, arrow_select);
	wprintw_quit(win, 5, arrow_select);
	wrefresh(win);
}
groupinfo search_groupid(WINDOW* win, int arrow) {

	groupinfo g;

	int group_file = 0;
	int a = 0;
	DIR* u_dir = opendir("..");
	struct dirent* u_ent;
	if (u_dir != NULL) {
		while ((u_ent = readdir(u_dir)) != NULL) {
			if (u_ent->d_type == DT_REG && strcmp(u_ent->d_name, "group.txt") ==0) {
				group_file = 1;
			}
		}
	}
	closedir(u_dir);


	int fd;
	int index_g = 0;

	// group.txt가 있는 경우
	if (group_file == 1) {

		if (chdir("..") == -1) {
			perror("chdir");
			exit(1);
		}

		fd = open("group.txt", O_RDONLY);

		// 커서 맨 앞으로
		lseek(fd, 0, SEEK_SET);

		// 읽을 구조체 선언하기
		groupinfo rd;

		// 저장할 구조체 배열 선언하기
		groupinfo save[12];

		// group.txt 파일 읽기
		// groupinfo 단위로 읽어옴
		while (read(fd, &rd, sizeof(groupinfo))) {
			strcpy(save[index_g].group_name, rd.group_name);
			save[index_g].group_users = rd.group_users;
			index_g++;
		}
		
		chdir("./no_group");
		return save[arrow - 1];
	}
	
}

void group_join(WINDOW* win) {
	int c;
	int arrow_select = 1;
	int index_group = 0;


	
	char groupid[30];
	groupinfo gp;

	while (1) {
		index_group = menu3_screen_join(win, c, arrow_select);
		c = wgetch(win);
		arrow_select = arrow_convert(c, arrow_select, index_group + 1);
		if (c == '\n') {
			if (1 <= arrow_select && arrow_select <= index_group) {
				gp = search_groupid(win, arrow_select);
				wrefresh(win);
				strcpy(groupid, gp.group_name);
				break;
			}
			else if (arrow_select == index_group + 1) {
				return;
			}
		}
	}

	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(win);
	echo();

	// 사용자 이름 복사
	char user[30] = { 0 };
	strcpy(user, UID);

	// 현재 경로 ==  ./users

	// ./users로 디렉토리 경로 변경
	if (chdir("..") != 0) {
		perror("chdir");
		return;
	}

	// ./users/no_group 안에 사용자가 있는지 -> 사용자가 아무 그룹에 속하지 않은 상태
	int user_flag = 0; // ./users 안에 생성된 그룹들이랑 같은지 표시하는 flag
	DIR* u_dir = opendir("./no_group");
	struct dirent* u_ent;
	if (u_dir != NULL) {
		while ((u_ent = readdir(u_dir)) != NULL) {
			if (u_ent->d_type == DT_DIR && strcmp(u_ent->d_name, user) == 0) {
				user_flag = 1; // no_group안에 사용자가 있음
			}
		}
	}
	closedir(u_dir);

	// 사용자는 그룹에 속한 상황
	if (user_flag == 0) {
		mvwprintw(win, 5, 2, "You ALREADY have a GROUP !!!");
		mvwprintw(win, 6, 2, "LEAVE group first then try again !");
		mvwprintw(win, 8, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 2;
			mvwprintw(win, 10, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}

	// ./users 안에 같은 그룹이 있는지 검사
	int group_flag = 0; // ./users 안에 생성된 그룹들이랑 같은지 표시하는 flag
	DIR* users_dir = opendir(".");
	struct dirent* ent;
	if (users_dir != NULL) {
		while ((ent = readdir(users_dir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1; // 이미 생성된 그룹 발견
			}
		}
	}
	closedir(users_dir);

	// users.txt 파일에 사용자 그룹 정보랑 일치하는지 확인
	// users.txt는 열려있음. users.txt의 fd를 ufd에 저장
	int ufd = usersFd;

	// 정보 읽을 구조체 선언
	Studyuser j_user;

	// 유저가 속한 그룹과 join하려는 그룹과 다른지 저장하는 flag
	int group_cmp = 0;

	// 커서 맨 앞으로 돌리고 구조체 단위로 정보 읽어가며 user_name 찾기
	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		// 파일에서 유저 찾기
		if (strcmp(j_user.user_ID, user) == 0) {
			// 유저의 그룹이랑 join하려는 그룹이랑 같음
			if (strcmp(j_user.group_ID, NO_GROUP) == 0) {
				group_cmp = 1;
			}
		}
	}
	// 사용자가 들어가고자 하는 그룹이 이미 있고, 지금은 사용자가 no_group임
	if (group_flag == 1 && group_cmp == 1) {

		// oldpath 경로 만들기
		char old_path[50] = { 0 };
		strcpy(old_path, "./no_group/");
		strcat(old_path, user);

		// newpath 경로 만들기
		char new_path[50] = { 0 };
		strcpy(new_path, "./");
		strcat(new_path, groupid);
		strcat(new_path, "/");
		strcat(new_path, user);

		int fd2;
		// group.txt 파일 오픈
		fd2 = open("group.txt", O_RDWR);

		// 파일을 구조체 단위로 읽고 그룹 이름이 중복되는 것이 있는지 검사
		int isgroup = 0;
		lseek(fd2, 0, SEEK_SET);

		// 읽을 구조체 선언하기
		groupinfo rd;
		// 쓸 구조체 선언하기
		groupinfo sa;

		int down_ten = 0;
		while (read(fd2, &rd, sizeof(groupinfo))) {
			// group.txt에 그룹 찾기
			if (strcmp(rd.group_name, groupid) == 0) {
				// 그룹 제한 10명 검사
				if (rd.group_users <= 9) {
					// 한명 더 늘리기
					sa.group_users = rd.group_users + 1;
					strcpy(sa.group_name, rd.group_name);
					lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
					write(fd2, &sa, sizeof(groupinfo));
					down_ten = 1;
					break;
				}
				else {
					// 사용자에게 그룹이 없다고 말하기
					// 메뉴 돌아가기

					mvwprintw(win, 5, 2, "There is NO GROUP !!!");
					mvwprintw(win, 6, 2, "Enter a PROPER group name !");
					mvwprintw(win, 8, 2, "Returning to group menu...");
					wrefresh(win);

					for (int t = 1; t <= 3; t++) {
						sleep(1);
						int y = 5 * t - 2;
						mvwprintw(win, 10, y, " %d ...", 4 - t);
						wrefresh(win);
					}
					sleep(1);
					wclear(win);
					return;
				}

			}
		}
		close(fd2);
		// 그룹에 10이하이면
		if (down_ten == 1) {
			// 현재 경로 ./users
			// ./users/no_group/user 에서 ./users/groupid/user로 디렉토리 경로 바꾸기

			// 디렉토리 경로 바꾸기 실패
			if (rename(old_path, new_path) != 0) {
				perror("rename");
				return;
			}
			// 디렉토리 경로 바꾸기 성공
			else {

				// users.txt 파일 변경
				int ufd = usersFd;
				int read_st = 0;
				Studyuser j_user;

				lseek(ufd, 0, SEEK_SET);
				while (read(ufd, &j_user, sizeof(Studyuser))) {
					if (strcmp(j_user.user_ID, user) == 0) {
						strcpy(j_user.group_ID, groupid); // GROUP ID is copy to j_user
						lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
						write(ufd, &j_user, sizeof(Studyuser)); // replacing
						wrefresh(win);
					}
				}

				mvwprintw(win, 3, 2, "You have JOIN <%s> !!!", groupid);
				mvwprintw(win, 5, 2, "Returning to group menu...");
				wrefresh(win);

				for (int t = 1; t <= 3; t++) {
					sleep(1);
					int y = 5 * t - 1;
					mvwprintw(win, 7, y, " %d ...", 4 - t);
					wrefresh(win);
				}
				sleep(1);
				wclear(win);
				return;
				wrefresh(win);
			}
		}

	}
	// ./users 경로에 그룹 디렉토리가 없어 생성해야함
	if (group_flag == 0) {
		// 사용자에게 돌아가서 create 하라고 하기
		mvwprintw(win, 5, 2, "Go back and CREATE group <%s> !!!", groupid);
		mvwprintw(win, 7, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 1;
			mvwprintw(win, 9, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}
	if (group_cmp == 0) {
		// 사용자가 no_group에 속하지 않음 == 이미 그룹이 있음
		mvwprintw(win, 5, 2, "You ALREADY have a GROUP !!!");
		mvwprintw(win, 6, 2, "LEAVE group first then try again !");
		mvwprintw(win, 8, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 1;
			mvwprintw(win, 10, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
	}
	Studyuser s_user;
	lseek(usersFd, -sizeof(Studyuser), SEEK_CUR);
	read(usersFd, &s_user, sizeof(Studyuser));

	chdir(s_user.group_ID);

	
	char menu;
	while ((menu = wgetch(win)) != 'q');

	wclear(win);
	wrefresh(win);
	return;
}

int menu3_screen_join(WINDOW* win, int key, int arrow_select) {

	wfill(win, 6, 1, 34, 56, " ");
	mvwprintw(win, 3, 2, "Group - Join");
	wrefresh(win);
	noecho();

	// getcwd를 사용해서 ./users/no_group 인 것을 확인함
	// ./users로 디렉토리 경로 변경
	if (chdir("..") != 0) {
		perror("chdir");
		return 0;
	}
	// ./users 인 현재 디렉토리 열고
	// group.txt 파일을 열고
	int group_file = 0;
	DIR* u_dir = opendir(".");
	struct dirent* u_ent;
	if (u_dir != NULL) {
		while ((u_ent = readdir(u_dir)) != NULL) {
			if (u_ent->d_type == DT_REG && strcmp(u_ent->d_name, "group.txt") == 0) {
				group_file = 1;
			}
		}
	}
	closedir(u_dir);

	int fd;
	int index_g = 0;

	// group.txt가 있는 경우
	if (group_file == 1) {

		fd = open("group.txt", O_RDONLY);

		// 커서 맨 앞으로
		lseek(fd, 0, SEEK_SET);

		// 읽을 구조체 선언하기
		groupinfo rd;

		// 저장할 구조체 배열 선언하기
		groupinfo save[12];

		// group.txt 파일 읽기
		// groupinfo 단위로 읽어옴
		while (read(fd, &rd, sizeof(groupinfo))) {
			strcpy(save[index_g].group_name, rd.group_name);
			save[index_g].group_users = rd.group_users;
			index_g++;
		}
		int ad = 0;
		int a = 0;
		mvwprintw(win, 6, 2, "Here is a list of groups you can join !");
		for (int i = 0; i < index_g; i++) {
			char temp[20] = { 0 };

			char str1[2];
			str1[0] = (i+1) + '0';
			char str2[2] = "";
			str2[0] = save[i].group_users + '0';
			str2[1] = '\0';
			str1[1] = '\0';

			strcpy(temp, str1);
			strcat(temp, " : ");
			strcat(temp, save[i].group_name);
			strcat(temp, "( ");
			strcat(temp, str2);
			strcat(temp, " )");
			// 1번부터 index까지
			mvwprintw_standout(win, i + 8 + a, 2, temp, i+1, arrow_select);
			a++;
			if (i == index_g - 1) {
				ad = a;
			}
		}
		wprintw_quit(win, index_g + 1, arrow_select);
		wrefresh(win);
	}
	else {
		int arrow_select = 1;
		mvwprintw(win, 5, 2, "You ALREADY have a GROUP !!!");
		mvwprintw(win, 6, 2, "LEAVE group first then try again !");
		mvwprintw(win, 8, 2, "Returning to group menu...");
		wprintw_quit(win, 1, arrow_select);
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 2;
			mvwprintw(win, 10, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return 0;
		
	}
	// 사용자에게 보여줌
	Studyuser s_user;
	lseek(usersFd, -sizeof(Studyuser), SEEK_CUR);
	read(usersFd, &s_user, sizeof(Studyuser));
	chdir(s_user.group_ID);

	int z = index_g;

	return z;
}



void group_create(WINDOW* win)
{
	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(win);

	echo();

	// 사용자 이름 복사
	char user[30] = { 0 };
	strcpy(user, UID);

	// 생성할 그룹 이름 받기
	//char groupid[30] = { 0 };
	char* groupid = malloc(sizeof(char*) * 30);
	mvwprintw(win, 3, 2, "Enter GROUP ID that you want to create : ");
	mvwgetstr(win, 3, 43, groupid);
	wrefresh(win);

	/*char path[50];
	getcwd(path, 50);
	mvwprintw(win, 19, 2, "path : %s", path);
	wrefresh(win);
	sleep(3);*/

	// ./users 안에 생성된 그룹이 있는지 검사
	int group_flag = 0; // ./users 안에 생성된 그룹이 있는지 검사하는 flag
	DIR* users_dir = opendir("./users");
	struct dirent* ent;
	if (users_dir != NULL) {

		while ((ent = readdir(users_dir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1; // 이미 생성된 그룹 발견
			}
		}
	}
	closedir(users_dir);

	if (group_flag == 1) {
		mvwprintw(win, 5, 2, "<%s> already CREATED !!!", groupid);
		mvwprintw(win, 7, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 2;
			mvwprintw(win, 9, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		Studyuser s_user;
		return;
	}
	else // group_flag == 0, 생성된 그룹이 없을 때
	{
		// 현재 경로 ./users/no_group

		// 현재 경로 ./users로 변경하기
		if (chdir("..") != 0) {
			perror("chdir");
			return;
		}

		// ./users 안에 groupid 그룹을 생성한다
		if (mkdir(groupid, 0755) == -1) {
			chdir(groupid);
			perror("mkdir");
			return;
		}

		// oldpath 경로 만들기
		char old_path[50] = { 0 };
		strcpy(old_path, "./no_group/");
		strcat(old_path, user);

		// newpath 경로 만들기
		char new_path[50] = { 0 };
		strcpy(new_path, "./");
		strcat(new_path, groupid);
		strcat(new_path, "/");
		strcat(new_path, user);

		// 현재 경로 ./users
		// ./users/no_group/user 에서 ./users/groupid/user로 디렉토리 경로 바꾸기

		// 디렉토리 경로 바꾸기 실패  
		if (rename(old_path, new_path) != 0) {
			perror("rename");
			return;
		}
		// 디렉토리 경로 바꾸기 성공
		else {
			int fd1, fd2;

			// 구조체에 정보 넣기
			groupinfo add;
			strcpy(add.group_name, groupid);
			add.group_users = 1;


			// 파일이 없어서 fd != -1 -> 파일 생성됨
			if ((fd1 = open("group.txt", O_RDWR | O_CREAT | O_EXCL, 0755)) > 0) {
				wrefresh(win);
				// 파일에 구조체 단위로 쓰기
				lseek(fd1, 0, SEEK_SET);
				write(fd1, &add, sizeof(groupinfo));
			}
			else { // 파일이 있어서 fd == -1임
				wrefresh(win);
				// 파일을 읽고 쓰는 권한으로 열기
				fd2 = open("group.txt", O_RDWR | O_APPEND);

				// 파일을 구조체 단위로 읽고 그룹 이름이 중복되는 것이 있는지 검사
				int isgroup = 0;
				lseek(fd2, 0, SEEK_SET);

				// 읽을 구조체 선언하기
				groupinfo rd;
				while (read(fd2, &rd, sizeof(groupinfo))) {
					// 파일에 그룹이 존재하는 경우 = 이미 존재하는 그룹 -> 그러면 안됨
					if (strcmp(rd.group_name, groupid) == 0) {
						isgroup = 1;
						break;
					}
				}
				// 파일에 그룹이 존재하지 않는 경우
				if (isgroup == 0) {
					wrefresh(win);
					lseek(fd2, 0, SEEK_END);
					write(fd2, &add, sizeof(groupinfo));
				}
			}
			// users.txt 파일에 바뀐 group 정보 replace
			int ufd = usersFd;
			int read_st = 0;
			Studyuser j_user;

			lseek(ufd, 0, SEEK_SET);
			while (read(ufd, &j_user, sizeof(Studyuser))) {
				if (strcmp(j_user.user_ID, user) == 0) {
					strcpy(j_user.group_ID, groupid); // GROUP ID is copy to j_user
					lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
					write(ufd, &j_user, sizeof(Studyuser)); // replacing
				}
			}
			// fd 닫아주기
			close(fd1);
			close(fd2);

			// 현재 경로 ./users
			mvwprintw(win, 5, 2, "You have CREATE and JOIN <%s> !!!", groupid);
			mvwprintw(win, 7, 2, "Returning to group menu...");
			wrefresh(win);

			for (int t = 1; t <= 3; t++) {
				sleep(1);
				int y = 5 * t - 1;
				mvwprintw(win, 9, y, " %d ...", 4 - t);
				wrefresh(win);
			}
			sleep(1);
			wclear(win);
			return;
		}
	}
	/*char path2[50];
	getcwd(path2, 50);
	mvwprintw(win, 21, 2, "path2 : %s", path2);
	wrefresh(win);
	sleep(3);*/



	sleep(1);
	free(groupid);
	wrefresh(win);

	return;
}

void group_leave(WINDOW* win)
{

	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(win);

	echo();

	// 사용자 이름 복사
	char user[30] = { 0 };
	strcpy(user, UID);

	// 사용자에게 그룹 떠나는 여부 물어보기
	char yn;
	mvwprintw(win, 3, 2, "Do you want to LEAVE this group ? (y/n) : ");
	yn = mvwgetch(win, 3, 44);
	wrefresh(win);

	sleep(1);

	// 사용자가 그룹을 떠난다고 했을 때
	if (yn == 'y') {

		// users.txt 에서 사용자 찾고 groupid 가져오기
		int ufd = usersFd;
		int isgroup = 0;
		char groupid[30] = { 0 };

		Studyuser j_user;

		lseek(ufd, 0, SEEK_SET);
		while (read(ufd, &j_user, sizeof(Studyuser))) {
			if (strcmp(j_user.user_ID, user) == 0) {
				strcpy(groupid, j_user.group_ID); // GROUP ID copy
				isgroup = 1;
				break;
			}
		}

		// users.txt에 사용자가 있을 때
		if (isgroup == 1) {
			// 이미 사용자는 어떤 그룹에 속하지 않음
			if (strcmp(groupid, NO_GROUP) == 0) {
				// 이미 속하지 않았다고 사용자한테 말하기
				// 메뉴로 돌아가기
				mvwprintw(win, 5, 2, "You ALREADY don't have a GROUP !!!");
				mvwprintw(win, 6, 2, "CREATE of JOIN a group first then try again !");
				mvwprintw(win, 8, 2, "Returning to group menu...");
				wrefresh(win);

				for (int t = 1; t <= 3; t++) {
					sleep(1);
					int y = 5 * t - 1;
					mvwprintw(win, 10, y, " %d ...", 4 - t);
					wrefresh(win);
				}
				sleep(1);
				wclear(win);
				return;
			}
			// 사용자는 그룹에 속함
			else {

				/*char path[50];
				getcwd(path, 50);
				mvwprintw(win, 20, 2, "path : %s", path);
				wrefresh(win);
				sleep(10);*/

				// users.txt 변경 정보 적용
				Studyuser r_user;

				lseek(ufd, 0, SEEK_SET);
				while (read(ufd, &r_user, sizeof(Studyuser))) {
					if (strcmp(r_user.user_ID, user) == 0) {
						// 사용자 그룹 탈퇴 -> group id : no_group
						strcpy(r_user.group_ID, NO_GROUP); // GROUP ID copy
						lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
						// 사용자 변경 정보 덮어쓰기
						write(ufd, &r_user, sizeof(Studyuser));
					}
				}
				// 현재 작업 경로 확인

				// group.txt 변경하기
				int fd2;
				// group.txt 파일 오픈
				fd2 = open("group.txt", O_RDWR | O_APPEND);

				if (fd2 != -1) {
					// 파일을 구조체 단위로 읽고 그룹 이름이 중복되는 것이 있는지 검사
					int isgroup = 0;
					lseek(fd2, 0, SEEK_SET);

					// 읽을 구조체 선언하기
					groupinfo rd;
					int no_one = 0;
					while (read(fd2, &rd, sizeof(groupinfo))) {
						// group.txt에 그룹 찾기
						if (strcmp(rd.group_name, groupid) == 0) {
							// 그룹 제한 10명 검사 (남은 인원이 한명이 아닐 때)
							if (rd.group_users != 1) {
								// 한명 줄이기
								rd.group_users -= 1;
								lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
								write(fd2, &rd, sizeof(groupinfo));
								break;
							}
							// 사용자 1명 밖에 안남았을 때
							else {
								no_one = 1; // 그룹의 인원이 1명 즉 사용자 밖에 남지 않았을 때
								//group.txt에서 그 그룹 정보 지우기
								// 구조체 초기화 
								memset(&rd, 0, sizeof(groupinfo));
								lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
								write(fd2, &rd, sizeof(groupinfo));
							}

						}
					}

					// ./users/groupid/user_name 경로를 ./users/no_group/user_name으로 변경

					// newpath 경로 만들기
					char new_path[50] = { 0 };
					strcpy(new_path, "./no_group/");
					strcat(new_path, user);

					// oldpath 경로 만들기
					char old_path[50] = { 0 };
					strcpy(old_path, "./");
					strcat(old_path, groupid);
					strcat(old_path, "/");
					strcat(old_path, user);

					// 현재 경로 ./users
					// ./users/no_group/user 에서 ./users/groupid/user로 디렉토리 경로 바꾸기

					// 디렉토리 경로 바꾸기 실패
					if (rename(old_path, new_path) != 0) {
						perror("rename");
						return;
					}
					else {
						// 경로 잘 바꾸어짐
						mvwprintw(win, 5, 2, "You left the GROUP !!!");
						mvwprintw(win, 7, 2, "Returning to group menu...");
						wrefresh(win);

						for (int t = 1; t <= 3; t++) {
							sleep(1);
							int y = 5 * t - 1;
							mvwprintw(win, 9, y, " %d ...", 4 - t);
							wrefresh(win);
						}
						sleep(1);
						wclear(win);
						return;
					}

					// 실행 안되면 getcwd로 경로 확인하면서 해결하기
				}
				// group.txt 파일을 열지 못할 경우
				else {
					perror("open");
					wclear(win);
					sleep(3);
					return;
				}
				close(fd2);
			}

		}

	}
	// 사용자가 그룹을 떠나지 않는다고 했을 때
	else if (yn == 'n')
	{
		mvwprintw(win, 5, 2, "You choose to NOT LEAVE this group !!!");
		mvwprintw(win, 7, 2, "Returning to group menu...");
		wrefresh(win);

		for (int t = 1; t <= 3; t++) {
			sleep(1);
			int y = 5 * t - 1;
			mvwprintw(win, 9, y, " %d ...", 4 - t);
			wrefresh(win);
		}
		sleep(1);
		wclear(win);
		return;
	}

	char menu;
	while ((menu = wgetch(win)) != 'q');

	chdir("./no_group");



	wclear(win);
	wrefresh(win);
	return;

}
void group_rank(WINDOW* win)
{

	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(win);

	echo();

	char groupid[30] = { 0 };
	char userid[30] = { 0 };

	mvwprintw(win, 3, 2, "Enter your USER ID : ");
	mvwgetstr(win, 3, 24, userid);

	//groupid 가지고 오기
	int ufd = usersFd;
	int read_st = 0;
	Studyuser j_user;
	int uflag = 0;
	int i = 0;

	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		if (strcmp(j_user.user_ID, userid) == 0) {
			uflag = 1; // userid is found
			strcpy(groupid, j_user.group_ID);
			//mvwprintw(win, 5, 2, "Your group id is %s", groupid);
			wrefresh(win);
			// GROUP ID is copy to j_user
		}
	}
	if (uflag == 1) { // 사용자 그룹을 얻었을 때
		//2. users 경로에서 group_name을 찾고 그 디렉토리를 연다
		int users_flag = 0;
		int group_flag = 0;
		char g_dir[60] = { 0 };

		// users/group_name 열어서 user 조회하기
		strcpy(g_dir, "./");
		strcat(g_dir, groupid);

		usertime rank[10] = { 0 };

		//char user_name[10][10];
		DIR* groupdir = opendir(g_dir); // ./user/group_name 디렉토리 열기
		if (groupdir != NULL) { // NULL값 검사
			struct dirent* ent;
			while ((ent = readdir(groupdir)) != NULL) {
				if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
					users_flag = 1;
					wrefresh(win);
					strncpy(rank[i].user_name, ent->d_name, sizeof(ent->d_name));
					i++;
				}
			}
			closedir(groupdir);
		}
		else {
			perror("Error opening group directory.\n");
			exit(1);
		}
		// 오늘 날짜 구하기
		time_t t = time(NULL);
		struct tm* now = localtime(&t);

		char dateStr[40] = { 0 };
		snprintf(dateStr, sizeof(dateStr), "%04d%02d%02d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

		// user_name 디렉토리에 들어가서 파일 존재 확인
		struct stat buf;
		char u_dir[60] = { 0 }; //users/group_name/users_name
		int today_flag = 0;

		for (int s = 0; s < i; s++) {

			strcpy(u_dir, g_dir);
			strcat(u_dir, "/");
			strcat(u_dir, rank[s].user_name);

			DIR* userDir = opendir(u_dir);
			if (u_dir != NULL) {
				struct dirent* u_entry;
				while ((u_entry = readdir(userDir)) != NULL) {
					if (strcmp(u_entry->d_name, dateStr) == 0) { // finding today record
						today_flag = 1;
					}
				}
				if (today_flag == 1) { // today file not found
					//파일열기
					int fd; // file descriptor 선언
					timelog todaytime; //timelog 구조체 todaytime 선언;

					char f_dir[100] = { 0 };// to save file path
					strcat(f_dir, u_dir);
					strcat(f_dir, "/");
					strcat(f_dir, dateStr); // making file path 

					if ((fd = open(f_dir, O_RDWR)) != -1) {
						//printf("calculating time...\n"); 
						//파일 구조체 단위로 읽기
						while (read(fd, &todaytime, sizeof(timelog))) {
							rank[s].study_time += todaytime.studytime;
						}
						close(fd);
					}
					else {
						closedir(userDir);
						break;
					}
				}
				else {
					rank[s].study_time = 0;
				}
			}
			closedir(userDir);
		}

		// rank 구조체 sorting 하기
		usertime temp;
		for (int y_index = 0; y_index < i; y_index++) {
			for (int x_index = 0; x_index < i - 1; x_index++) {
				if (rank[x_index].study_time < rank[x_index + 1].study_time) {
					temp = rank[x_index];
					rank[x_index] = rank[x_index + 1];
					rank[x_index + 1] = temp;
				}
			}
		}

		// sorting 한 결과 보여주기
		mvwprintw(win, 5, 2, "Today's <%s> ranking !", groupid);
		int x = 7;
		int y = 2;
		for (int n = 0; n < i; n++) {
			mvwprintw(win, x, y, "%d.\t%s\t%lf", n + 1, rank[n].user_name, rank[n].study_time);
			if (strcmp(rank[n].user_name, userid) == 0) {
				mvwprintw(win, x, 30, "<-");
			}
			x++;
			wrefresh(win);
		}

	}

	char menu;
	while ((menu = wgetch(win)) != 'q');

	wclear(win);
	wrefresh(win);
	return;

}



