#define USERS_INFO_DIR "users"
#define USERS_INFO_FILE "users.txt"
#define NO_GROUP "no_group"

extern int usersFd;
extern char UID[11];

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

typedef struct usertime { //group ranking 때 필요한 구조체
	char user_name[50];
	double study_time;
}usertime;

typedef struct groupinfo {
	char group_name[50];
	int group_users;
}groupinfo;

void ID_check(int argc, char* argv[]);
void login(int argc, char* argv[]);
void initial_set();
void unsetup();
void set_forFirstRun();
