#include <curses.h>

#define USERS_INFO_DIR "users"
#define USERS_INFO_FILE "users.txt"
#define NO_GROUP "no_group"

#define JANUARY 0
#define DECEMBER 11
#define SECONDS_PER_DAY 86400

typedef struct Studyuser
{
	char user_ID[11];
	char group_ID[11];
	time_t signup;// 가입 일자
	time_t lastlogin;// 최종 접속 시간
} Studyuser;

typedef struct timelog // 공부 시간 기록을 저장하는 구조체
{
	// Studyuser info; 
	char subject[30];
	time_t start_time; // 공부 시작 시간
	time_t finish_time; // 공부 종료 시간
	double studytime; // 공부 시간
} timelog;


extern char UID[11];
extern int user_dead;

void wdraw_timebar(WINDOW* win, int y_start, int x_start);
void stats_select(WINDOW* win);
void stats_screen(WINDOW* win, int arrow_select);

void daystats(WINDOW* win);
void weekstats(WINDOW* win);
void monthstats(WINDOW* win);
void create_record(WINDOW* win, int year, int month, int day);
void display_daystats(WINDOW* win, int year, int month, int day);
void display_weekstats(WINDOW* win, time_t today);
void display_monthstats(WINDOW* win, struct tm statmonth_tm);

void settings_select(WINDOW* win);
void settings_screen(WINDOW* win, int arrow_select);
void myprofile(WINDOW* win);
void display_profile(WINDOW* win, Studyuser s_user, int arrow_select);
void display_help(WINDOW* win);
void delete_account(WINDOW* win);

