#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX 50
#define BUFFER 20

typedef struct user_info{
	char group[MAX];	
	char name[MAX];
	char start[MAX];
	char end[MAX];
}UserInfo;

UserInfo user_input();
char* combine_info(UserInfo);

int main(void){
	
	char* all_str;
	int fd;

	UserInfo new_info;
	new_info = user_input();

	all_str = combine_info(new_info);
	//printf("%s", all_str);
	
	if(0<(fd = open("user_info.txt", O_RDWR|O_APPEND|O_CREAT,0664))){
		write(fd, all_str, strlen(all_str));
		close(fd);
	}
	else{
		printf("failed to open file\n");
	}

	char f_group[MAX];
	char buf[BUFFER];
	int g_flag =0;
	int ffd;
	printf("What is the name of the group do you want to find : ");
	scanf("%s", f_group);

	/*if((ffd =open("user_info.txt", O_RDONLY)) != -1){
		
		while(read(ffd, buf, BUFFER) == BUFFER){
			if(strstr(buf, f_group) != NULL){
				g_flag =1;
			}
		}
	}
	else fprintf(stderr, "failed to open file\n");
	if(g_flag ==1){
		printf("group found\n");
	}*/
	char copy_info[MAX][MAX];
	char temp[64];
	int index =0;
	FILE* p_file = fopen("user_info.txt", "rt");
	if(p_file != NULL){
		while(fgets(temp, sizeof(temp), p_file)){
			strcpy(copy_info[index], temp);
			printf("%d : %s", index+1, temp);
			index ++;
		}
		fclose(p_file);
	}	


	return 0;
}

UserInfo user_input(){

	UserInfo new;

	printf("Input group name : ");
	scanf("%s", new.group);

	printf("Input user name : ");
	scanf("%s", new.name);

	printf("Input start time : ");
	scanf("%s", new.start);

	printf("Input end time : ");
	scanf("%s", new.end);

	return new;
}
char* combine_info(UserInfo new_info){
	
	char* str = malloc(sizeof(char) * 100);

	strcpy(str, new_info.group);
	
	strcat(str, " ");
	strcat(str, new_info.name);

	strcat(str," ");
	strcat(str, new_info.start);

	strcat(str," ");
	strcat(str, new_info.end);
	strcat(str,"\n");

	return str;
}


