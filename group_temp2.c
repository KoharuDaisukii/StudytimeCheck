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

typedef struct usertime { //group ranking �� �ʿ��� ����ü
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

	// group.txt�� �ִ� ���
	if (group_file == 1) {

		if (chdir("..") == -1) {
			perror("chdir");
			exit(1);
		}

		fd = open("group.txt", O_RDONLY);

		// Ŀ�� �� ������
		lseek(fd, 0, SEEK_SET);

		// ���� ����ü �����ϱ�
		groupinfo rd;

		// ������ ����ü �迭 �����ϱ�
		groupinfo save[12];

		// group.txt ���� �б�
		// groupinfo ������ �о��
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

	// ����� �̸� ����
	char user[30] = { 0 };
	strcpy(user, UID);

	// ���� ��� ==  ./users

	// ./users�� ���丮 ��� ����
	if (chdir("..") != 0) {
		perror("chdir");
		return;
	}

	// ./users/no_group �ȿ� ����ڰ� �ִ��� -> ����ڰ� �ƹ� �׷쿡 ������ ���� ����
	int user_flag = 0; // ./users �ȿ� ������ �׷���̶� ������ ǥ���ϴ� flag
	DIR* u_dir = opendir("./no_group");
	struct dirent* u_ent;
	if (u_dir != NULL) {
		while ((u_ent = readdir(u_dir)) != NULL) {
			if (u_ent->d_type == DT_DIR && strcmp(u_ent->d_name, user) == 0) {
				user_flag = 1; // no_group�ȿ� ����ڰ� ����
			}
		}
	}
	closedir(u_dir);

	// ����ڴ� �׷쿡 ���� ��Ȳ
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

	// ./users �ȿ� ���� �׷��� �ִ��� �˻�
	int group_flag = 0; // ./users �ȿ� ������ �׷���̶� ������ ǥ���ϴ� flag
	DIR* users_dir = opendir(".");
	struct dirent* ent;
	if (users_dir != NULL) {
		while ((ent = readdir(users_dir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1; // �̹� ������ �׷� �߰�
			}
		}
	}
	closedir(users_dir);

	// users.txt ���Ͽ� ����� �׷� ������ ��ġ�ϴ��� Ȯ��
	// users.txt�� ��������. users.txt�� fd�� ufd�� ����
	int ufd = usersFd;

	// ���� ���� ����ü ����
	Studyuser j_user;

	// ������ ���� �׷�� join�Ϸ��� �׷�� �ٸ��� �����ϴ� flag
	int group_cmp = 0;

	// Ŀ�� �� ������ ������ ����ü ������ ���� �о�� user_name ã��
	lseek(ufd, 0, SEEK_SET);
	while (read(ufd, &j_user, sizeof(Studyuser))) {
		// ���Ͽ��� ���� ã��
		if (strcmp(j_user.user_ID, user) == 0) {
			// ������ �׷��̶� join�Ϸ��� �׷��̶� ����
			if (strcmp(j_user.group_ID, NO_GROUP) == 0) {
				group_cmp = 1;
			}
		}
	}
	// ����ڰ� ������ �ϴ� �׷��� �̹� �ְ�, ������ ����ڰ� no_group��
	if (group_flag == 1 && group_cmp == 1) {

		// oldpath ��� �����
		char old_path[50] = { 0 };
		strcpy(old_path, "./no_group/");
		strcat(old_path, user);

		// newpath ��� �����
		char new_path[50] = { 0 };
		strcpy(new_path, "./");
		strcat(new_path, groupid);
		strcat(new_path, "/");
		strcat(new_path, user);

		int fd2;
		// group.txt ���� ����
		fd2 = open("group.txt", O_RDWR);

		// ������ ����ü ������ �а� �׷� �̸��� �ߺ��Ǵ� ���� �ִ��� �˻�
		int isgroup = 0;
		lseek(fd2, 0, SEEK_SET);

		// ���� ����ü �����ϱ�
		groupinfo rd;
		// �� ����ü �����ϱ�
		groupinfo sa;

		int down_ten = 0;
		while (read(fd2, &rd, sizeof(groupinfo))) {
			// group.txt�� �׷� ã��
			if (strcmp(rd.group_name, groupid) == 0) {
				// �׷� ���� 10�� �˻�
				if (rd.group_users <= 9) {
					// �Ѹ� �� �ø���
					sa.group_users = rd.group_users + 1;
					strcpy(sa.group_name, rd.group_name);
					lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
					write(fd2, &sa, sizeof(groupinfo));
					down_ten = 1;
					break;
				}
				else {
					// ����ڿ��� �׷��� ���ٰ� ���ϱ�
					// �޴� ���ư���

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
		// �׷쿡 10�����̸�
		if (down_ten == 1) {
			// ���� ��� ./users
			// ./users/no_group/user ���� ./users/groupid/user�� ���丮 ��� �ٲٱ�

			// ���丮 ��� �ٲٱ� ����
			if (rename(old_path, new_path) != 0) {
				perror("rename");
				return;
			}
			// ���丮 ��� �ٲٱ� ����
			else {

				// users.txt ���� ����
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
	// ./users ��ο� �׷� ���丮�� ���� �����ؾ���
	if (group_flag == 0) {
		// ����ڿ��� ���ư��� create �϶�� �ϱ�
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
		// ����ڰ� no_group�� ������ ���� == �̹� �׷��� ����
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

	// getcwd�� ����ؼ� ./users/no_group �� ���� Ȯ����
	// ./users�� ���丮 ��� ����
	if (chdir("..") != 0) {
		perror("chdir");
		return 0;
	}
	// ./users �� ���� ���丮 ����
	// group.txt ������ ����
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

	// group.txt�� �ִ� ���
	if (group_file == 1) {

		fd = open("group.txt", O_RDONLY);

		// Ŀ�� �� ������
		lseek(fd, 0, SEEK_SET);

		// ���� ����ü �����ϱ�
		groupinfo rd;

		// ������ ����ü �迭 �����ϱ�
		groupinfo save[12];

		// group.txt ���� �б�
		// groupinfo ������ �о��
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
			// 1������ index����
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
	// ����ڿ��� ������
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

	// ����� �̸� ����
	char user[30] = { 0 };
	strcpy(user, UID);

	// ������ �׷� �̸� �ޱ�
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

	// ./users �ȿ� ������ �׷��� �ִ��� �˻�
	int group_flag = 0; // ./users �ȿ� ������ �׷��� �ִ��� �˻��ϴ� flag
	DIR* users_dir = opendir("./users");
	struct dirent* ent;
	if (users_dir != NULL) {

		while ((ent = readdir(users_dir)) != NULL) {
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, groupid) == 0) {
				group_flag = 1; // �̹� ������ �׷� �߰�
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
	else // group_flag == 0, ������ �׷��� ���� ��
	{
		// ���� ��� ./users/no_group

		// ���� ��� ./users�� �����ϱ�
		if (chdir("..") != 0) {
			perror("chdir");
			return;
		}

		// ./users �ȿ� groupid �׷��� �����Ѵ�
		if (mkdir(groupid, 0755) == -1) {
			chdir(groupid);
			perror("mkdir");
			return;
		}

		// oldpath ��� �����
		char old_path[50] = { 0 };
		strcpy(old_path, "./no_group/");
		strcat(old_path, user);

		// newpath ��� �����
		char new_path[50] = { 0 };
		strcpy(new_path, "./");
		strcat(new_path, groupid);
		strcat(new_path, "/");
		strcat(new_path, user);

		// ���� ��� ./users
		// ./users/no_group/user ���� ./users/groupid/user�� ���丮 ��� �ٲٱ�

		// ���丮 ��� �ٲٱ� ����  
		if (rename(old_path, new_path) != 0) {
			perror("rename");
			return;
		}
		// ���丮 ��� �ٲٱ� ����
		else {
			int fd1, fd2;

			// ����ü�� ���� �ֱ�
			groupinfo add;
			strcpy(add.group_name, groupid);
			add.group_users = 1;


			// ������ ��� fd != -1 -> ���� ������
			if ((fd1 = open("group.txt", O_RDWR | O_CREAT | O_EXCL, 0755)) > 0) {
				wrefresh(win);
				// ���Ͽ� ����ü ������ ����
				lseek(fd1, 0, SEEK_SET);
				write(fd1, &add, sizeof(groupinfo));
			}
			else { // ������ �־ fd == -1��
				wrefresh(win);
				// ������ �а� ���� �������� ����
				fd2 = open("group.txt", O_RDWR | O_APPEND);

				// ������ ����ü ������ �а� �׷� �̸��� �ߺ��Ǵ� ���� �ִ��� �˻�
				int isgroup = 0;
				lseek(fd2, 0, SEEK_SET);

				// ���� ����ü �����ϱ�
				groupinfo rd;
				while (read(fd2, &rd, sizeof(groupinfo))) {
					// ���Ͽ� �׷��� �����ϴ� ��� = �̹� �����ϴ� �׷� -> �׷��� �ȵ�
					if (strcmp(rd.group_name, groupid) == 0) {
						isgroup = 1;
						break;
					}
				}
				// ���Ͽ� �׷��� �������� �ʴ� ���
				if (isgroup == 0) {
					wrefresh(win);
					lseek(fd2, 0, SEEK_END);
					write(fd2, &add, sizeof(groupinfo));
				}
			}
			// users.txt ���Ͽ� �ٲ� group ���� replace
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
			// fd �ݾ��ֱ�
			close(fd1);
			close(fd2);

			// ���� ��� ./users
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

	// ����� �̸� ����
	char user[30] = { 0 };
	strcpy(user, UID);

	// ����ڿ��� �׷� ������ ���� �����
	char yn;
	mvwprintw(win, 3, 2, "Do you want to LEAVE this group ? (y/n) : ");
	yn = mvwgetch(win, 3, 44);
	wrefresh(win);

	sleep(1);

	// ����ڰ� �׷��� �����ٰ� ���� ��
	if (yn == 'y') {

		// users.txt ���� ����� ã�� groupid ��������
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

		// users.txt�� ����ڰ� ���� ��
		if (isgroup == 1) {
			// �̹� ����ڴ� � �׷쿡 ������ ����
			if (strcmp(groupid, NO_GROUP) == 0) {
				// �̹� ������ �ʾҴٰ� ��������� ���ϱ�
				// �޴��� ���ư���
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
			// ����ڴ� �׷쿡 ����
			else {

				/*char path[50];
				getcwd(path, 50);
				mvwprintw(win, 20, 2, "path : %s", path);
				wrefresh(win);
				sleep(10);*/

				// users.txt ���� ���� ����
				Studyuser r_user;

				lseek(ufd, 0, SEEK_SET);
				while (read(ufd, &r_user, sizeof(Studyuser))) {
					if (strcmp(r_user.user_ID, user) == 0) {
						// ����� �׷� Ż�� -> group id : no_group
						strcpy(r_user.group_ID, NO_GROUP); // GROUP ID copy
						lseek(ufd, -sizeof(Studyuser), SEEK_CUR); // moving cursor to start point
						// ����� ���� ���� �����
						write(ufd, &r_user, sizeof(Studyuser));
					}
				}
				// ���� �۾� ��� Ȯ��

				// group.txt �����ϱ�
				int fd2;
				// group.txt ���� ����
				fd2 = open("group.txt", O_RDWR | O_APPEND);

				if (fd2 != -1) {
					// ������ ����ü ������ �а� �׷� �̸��� �ߺ��Ǵ� ���� �ִ��� �˻�
					int isgroup = 0;
					lseek(fd2, 0, SEEK_SET);

					// ���� ����ü �����ϱ�
					groupinfo rd;
					int no_one = 0;
					while (read(fd2, &rd, sizeof(groupinfo))) {
						// group.txt�� �׷� ã��
						if (strcmp(rd.group_name, groupid) == 0) {
							// �׷� ���� 10�� �˻� (���� �ο��� �Ѹ��� �ƴ� ��)
							if (rd.group_users != 1) {
								// �Ѹ� ���̱�
								rd.group_users -= 1;
								lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
								write(fd2, &rd, sizeof(groupinfo));
								break;
							}
							// ����� 1�� �ۿ� �ȳ����� ��
							else {
								no_one = 1; // �׷��� �ο��� 1�� �� ����� �ۿ� ���� �ʾ��� ��
								//group.txt���� �� �׷� ���� �����
								// ����ü �ʱ�ȭ 
								memset(&rd, 0, sizeof(groupinfo));
								lseek(fd2, -sizeof(groupinfo), SEEK_CUR);
								write(fd2, &rd, sizeof(groupinfo));
							}

						}
					}

					// ./users/groupid/user_name ��θ� ./users/no_group/user_name���� ����

					// newpath ��� �����
					char new_path[50] = { 0 };
					strcpy(new_path, "./no_group/");
					strcat(new_path, user);

					// oldpath ��� �����
					char old_path[50] = { 0 };
					strcpy(old_path, "./");
					strcat(old_path, groupid);
					strcat(old_path, "/");
					strcat(old_path, user);

					// ���� ��� ./users
					// ./users/no_group/user ���� ./users/groupid/user�� ���丮 ��� �ٲٱ�

					// ���丮 ��� �ٲٱ� ����
					if (rename(old_path, new_path) != 0) {
						perror("rename");
						return;
					}
					else {
						// ��� �� �ٲپ���
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

					// ���� �ȵǸ� getcwd�� ��� Ȯ���ϸ鼭 �ذ��ϱ�
				}
				// group.txt ������ ���� ���� ���
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
	// ����ڰ� �׷��� ������ �ʴ´ٰ� ���� ��
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

	//groupid ������ ����
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
	if (uflag == 1) { // ����� �׷��� ����� ��
		//2. users ��ο��� group_name�� ã�� �� ���丮�� ����
		int users_flag = 0;
		int group_flag = 0;
		char g_dir[60] = { 0 };

		// users/group_name ��� user ��ȸ�ϱ�
		strcpy(g_dir, "./");
		strcat(g_dir, groupid);

		usertime rank[10] = { 0 };

		//char user_name[10][10];
		DIR* groupdir = opendir(g_dir); // ./user/group_name ���丮 ����
		if (groupdir != NULL) { // NULL�� �˻�
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
		// ���� ��¥ ���ϱ�
		time_t t = time(NULL);
		struct tm* now = localtime(&t);

		char dateStr[40] = { 0 };
		snprintf(dateStr, sizeof(dateStr), "%04d%02d%02d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

		// user_name ���丮�� ���� ���� ���� Ȯ��
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
					//���Ͽ���
					int fd; // file descriptor ����
					timelog todaytime; //timelog ����ü todaytime ����;

					char f_dir[100] = { 0 };// to save file path
					strcat(f_dir, u_dir);
					strcat(f_dir, "/");
					strcat(f_dir, dateStr); // making file path 

					if ((fd = open(f_dir, O_RDWR)) != -1) {
						//printf("calculating time...\n"); 
						//���� ����ü ������ �б�
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

		// rank ����ü sorting �ϱ�
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

		// sorting �� ��� �����ֱ�
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



