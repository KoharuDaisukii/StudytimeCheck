#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>
#include <fcntl.h> 
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include "studytimecheck.h"
#include "varlib.h"

#define TEST 0

void menu2_1(WINDOW*);
void menu2_2(WINDOW*);
void menu2_3(WINDOW*);
void wsctotime_month(WINDOW* win, int seconds);

int monthlast_calculator(int month);


void wsctotime(WINDOW* win, int seconds)
{
	int day, hour, min;
	day = seconds / SECONDS_PER_DAY;
	min = seconds / 60;
	hour = (min / 60) % 24;
	min = min % 60;
	seconds = seconds % 60;
	
	if(day > 0)
		wprintw(win, "%dd %0dh %0dm %0ds", day, hour, min, seconds);
	else 
		wprintw(win, "%0dh %0dm %0ds", hour, min, seconds);
}

void wsctotime_month(WINDOW* win, int seconds)
{
	int hour, min;
	min = seconds / 60;
	hour = min / 60;
	seconds = seconds % 60;
	min = min % 60;
	if(seconds == 0)
	{
		wprintw(win, "    ---    ", hour, min, seconds);
		return;
	}
	wprintw(win, " %02dh%02dm%02ds ", hour, min, seconds);
}

void wdraw_timebar(WINDOW* win, int y_start, int x_start)
{
	mvwprintw(win, y_start, x_start, "|");
	wfill(win, y_start, x_start+1, y_start, x_start+52, "-");
	wprintw(win, "|");
}

void stats_select(WINDOW* win)
{
	int c;
	int arrow_select = 1;
	while (1)
	{
		stats_screen(win, arrow_select);
		c = wgetch(win);
		arrow_select = arrow_convert(c, arrow_select, 4);
		if (c == 'q') break;
		if (c == '1' || (c == '\n' && arrow_select == 1)) menu2_1(win);
		if (c == '2' || (c== '\n' && arrow_select == 2)) menu2_2(win);
		if (c == '3' || (c == '\n' && arrow_select == 3)) menu2_3(win);
		if (c == '4' || (c == '\n' && arrow_select == 4)) break;
		if (user_dead == 1) break;
	}
}

void stats_screen(WINDOW* win, int arrow_select)
{
	wclear(win);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+'); 
	
	mvwprintw(win, 3, 2, "Display stats");
	mvwprintw_standout(win, 7, 2, "1. Today's Studytime", 1, arrow_select);
	mvwprintw_standout(win, 10, 2, "2. This week's Studytime", 2, arrow_select);
	mvwprintw_standout(win, 13, 2, "3. This month's Studytime", 3, arrow_select);
	wprintw_quit(win, 4, arrow_select);
	wrefresh(win);
}

void daystats(WINDOW* win)
{
	time_t statdate = time(NULL);
	time_t today = statdate;
	struct tm* statdate_tm = localtime(&statdate);

	int today_year = statdate_tm->tm_year + 1900;
	int today_month = statdate_tm->tm_mon + 1;
	int today_day = statdate_tm->tm_mday;

	// 통계를 보여줄 년월일 변수
	int stat_year = statdate_tm->tm_year + 1900;
	int stat_month = statdate_tm->tm_mon + 1;
	int stat_day = statdate_tm->tm_mday;

	int c;
	while (1)
	{
		mvwprintw(win, 3, 2, "Display stats (Arrow keys for control)");
		if (stat_year == today_year && stat_month == today_month && stat_day == today_day)
			mvwprintw(win, 5, 2, "Studytime during Today(%04d-%02d-%02d)", stat_year, stat_month, stat_day);
		else
			mvwprintw(win, 5, 2, "Studytime of %04d-%02d-%02d           ", stat_year, stat_month, stat_day);
		wprintw_quit(win, 0, 0);	
		wfill(win, 6, 1, 34, 56, " ");
		display_daystats(win, stat_year, stat_month, stat_day, 0);
		c = wgetch(win);	
		
		if (c == '\n') break;
		if (c == ARROW_DOWN || c == ARROW_LEFT)
		{
			statdate -= SECONDS_PER_DAY;
			statdate_tm = localtime(&statdate);
			stat_year = statdate_tm->tm_year + 1900;
			stat_month = statdate_tm->tm_mon + 1;
			stat_day = statdate_tm->tm_mday;
		}
		if (statdate < today && (c == ARROW_UP || c == ARROW_RIGHT))
		{
			statdate += SECONDS_PER_DAY;
			statdate_tm = localtime(&statdate);
			stat_year = statdate_tm->tm_year + 1900;
			stat_month = statdate_tm->tm_mon + 1;
			stat_day = statdate_tm->tm_mday;
		}
	}
	wclear(win);
}

void create_record(WINDOW* win, int year, int month, int day)
{
	char statfile[256];
	sprintf(statfile, "%04d%02d%02d.txt", year, month, day);

	int fd1;
	if ((fd1 = creat(statfile, 0777)) == -1)
	{
		perror("open");
		exit(22);
	}

	char subject[9][30] = {"Computer Architecture", "System Programming", "Algorithm Practice", "Java Programming", "Databases", "Operating System", "Network Programming", "Data Structure", "Compiler"};
	srand(time(NULL));
	int count = rand() % 10;
	
	for(int i=0; i<count; i++)
	{
		timelog templog = {0, };
		strcpy(templog.subject, subject[rand() % 9]);
		templog.start_time = 0;
		templog.finish_time = rand() % 9600 + 2;
		templog.studytime = (double)templog.finish_time - templog.start_time;
		write(fd1, &templog, sizeof(timelog));
	}
	
	close(fd1);
}

void display_daystats(WINDOW* win, int year, int month, int day, int external)
{
	char statfile[256];
	sprintf(statfile, "%04d%02d%02d.txt", year, month, day);
	
	char UID_dir[256];
	sprintf(UID_dir, "%s/%s", ".", UID);
	if (chdir(UID_dir) == -1)
	{
		perror("chdir");
		exit(21);
	}
	
#if TEST
	create_record(win, year, month, day);
#endif

	int fd2;
	double total = 0.0;
	double max = 0.0;
	if ((fd2 = open(statfile, O_RDONLY)) == -1)
	{
		mvwprintw(win, 6, 2, "Total studytime: ", total);
		wsctotime(win, total);
		chdir("..");
		return;
	}

	int subject_count = 0;
	timelog subjectlog[9] = {0, };
	
	timelog templog;
	while (read(fd2, &templog, sizeof(timelog)) >= sizeof(timelog))
	{
		int subject_i;
		total += templog.studytime;
		for (subject_i = 0; subject_i < subject_count; subject_i++)
		{
			if (strcmp(subjectlog[subject_i].subject, templog.subject) == 0)
			{
				subjectlog[subject_i].studytime += templog.studytime;
				break;
			}
		}
		
		if (subject_i != subject_count)
			continue;
		strcpy(subjectlog[subject_count].subject, templog.subject);
		subjectlog[subject_count++].studytime += templog.studytime;
	}
	close(fd2);
	wfill(win, 6, 1, 34, 55, " ");
	for (int subject_i = 0; subject_i < subject_count; subject_i++)
	{
		if(subjectlog[subject_i].studytime > max)
			max = subjectlog[subject_i].studytime;
	}
	
	mvwprintw(win, 6, 2, "Total studytime: ");
	wsctotime(win, total);
	for (int subject_i = 0; subject_i < subject_count; subject_i++)
	{
		mvwprintw(win, 8+subject_i*3, 2, "%d. %s: ", subject_i + 1, subjectlog[subject_i].subject);
		wsctotime(win, subjectlog[subject_i].studytime);
		wdraw_timebar(win, 9+subject_i*3, 2);
		mvwprintw(win, 8+subject_i*3, 45, "%.0f", subjectlog[subject_i].studytime);
		if(subjectlog[subject_i].studytime == 0)
			continue;
		if(subjectlog[subject_i].studytime == max)
		{
			wfill(win, 9+subject_i*3, 3, 9+subject_i*3, 53, "%%");
			continue;
		}
		wfill(win, 9+subject_i*3, 3, 9+subject_i*3, 3 + subjectlog[subject_i].studytime / (max / 51), "%%");
	}
	wrefresh(win);
	
	if(external == 1)
	{
		mvwprintw(win, 5, 2, "Studytime of %04d-%02d-%02d           ", year, month, day);
		int c;
		while((c = wgetch(win)) != '\n');
	}
	
	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
}

void weekstats(WINDOW* win)
{
	time_t endweek = time(NULL);
	time_t today = endweek;
	time_t startweek = endweek - (SECONDS_PER_DAY * 6);
	// localtime은 static data구나
	struct tm endweek_tm; localtime_r(&endweek, &endweek_tm);
	struct tm startweek_tm; localtime_r(&startweek, &startweek_tm);

	// int today_year = endweek_tm->tm_year + 1900;
	// int today_month = endweek_tm->tm_mon + 1;
	// int today_day = endweek_tm->tm_mday;
	
	int c;
	int arrow_select = 8;
	while (1)
	{
		wfill(win, 6, 1, 34, 55, " ");
		mvwprintw(win, 3, 2, "Display stats (Arrow keys for control)");
		mvwprintw(win, 5, 2, "Studytime of %04d-%02d-%02d ~ %04d-%02d-%02d", startweek_tm.tm_year + 1900, startweek_tm.tm_mon + 1, startweek_tm.tm_mday, endweek_tm.tm_year + 1900, endweek_tm.tm_mon + 1, endweek_tm.tm_mday);
		wprintw_quit(win, 8, arrow_select);
		display_weekstats(win, endweek, arrow_select);

		c = wgetch(win);
		arrow_select = arrow_convert(c, arrow_select, 8);
		if (c == '\n') 
		{
			if(arrow_select == 8)
				break;
			time_t startweek_copy = startweek + SECONDS_PER_DAY * (arrow_select - 1);
			struct tm startweek_tm_copy; localtime_r(&startweek_copy, &startweek_tm_copy);

			wfill(win, 4, 1, 34, 58, " ");
			display_daystats(win, startweek_tm_copy.tm_year + 1900, startweek_tm_copy.tm_mon + 1, startweek_tm_copy.tm_mday, 1);
		}
		if (c == ARROW_LEFT)
		{
			endweek -= SECONDS_PER_DAY;
			startweek -= SECONDS_PER_DAY;
		}
		if (endweek < today && c == ARROW_RIGHT)
		{
			endweek += SECONDS_PER_DAY;
			startweek += SECONDS_PER_DAY;	
			
		}
		localtime_r(&endweek, &endweek_tm);
		localtime_r(&startweek, &startweek_tm);
	}
	wclear(win);
}

void display_weekstats(WINDOW* win, time_t today, int arrow_select)
{
	char UID_dir[256];
	sprintf(UID_dir, "%s/%s", ".", UID);
	
	if (chdir(UID_dir) == -1)
	{
		perror("chdir");
		exit(21);
	}

	timelog weeklog[7];
	int year, month, day;
	double total = 0.0;
	double max = 0.0;
	for (int week_i = 6; week_i >= 0; week_i--)
	{
		char statfile[15];
		struct tm today_tm; localtime_r(&today, &today_tm);
		year = today_tm.tm_year + 1900;
		month = today_tm.tm_mon + 1;
		day = today_tm.tm_mday;
		sprintf(statfile, "%04d%02d%02d.txt", year, month, day);
		
		char whatdate[11];
		sprintf(whatdate, "%04d-%02d-%02d", year, month, day);
		mvwprintw_standout(win, 8 + week_i * 4, 2, whatdate, week_i + 1, arrow_select);
		wprintw(win, ": ");
		
		weeklog[week_i].studytime = 0.0;
		int fd;
		if ((fd = open(statfile, O_RDONLY)) == -1)
		{
			wsctotime(win, weeklog[week_i].studytime);
			wdraw_timebar(win, 9+week_i*4, 2);
			today -= SECONDS_PER_DAY;
			continue;
		}

		timelog templog;
		while (read(fd, &templog, sizeof(timelog)) >= sizeof(timelog))
		{
			weeklog[week_i].studytime += templog.studytime;
			total += templog.studytime;
		}
		close(fd);
		
		if(weeklog[week_i].studytime > max)
			max = weeklog[week_i].studytime;
		wsctotime(win, weeklog[week_i].studytime);
		wdraw_timebar(win, 9+week_i*4, 2);
		
		today -= SECONDS_PER_DAY;
	}
	mvwprintw(win, 6, 2, "Total studytime: ");
	wsctotime(win, total);
	
	for(int week_i = 0; week_i <= 6 ; week_i++)
	{
		if(weeklog[week_i].studytime == 0)
			continue;
		if(weeklog[week_i].studytime == max)
		{
			wfill(win, 9+week_i*4, 3, 9+week_i*4, 53, "%%");
			continue;
		}
		wfill(win, 9+week_i*4, 3, 9+week_i*4, 3+weeklog[week_i].studytime / (max / 51), "%%");
	}
	
	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
}

int monthlast_calculator(int month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 2:
		return 28;
	default:
		return 30;
	}
}

void monthstats(WINDOW* win)
{
	time_t statmonth = time(NULL);
	time_t today = statmonth;
	// localtime은 static data구나
	struct tm statmonth_tm;
	// int today_year = endweek_tm->tm_year + 1900;
	// int today_month = endweek_tm->tm_mon + 1;
	// int today_day = endweek_tm->tm_mday;
	
	int c;
	int arrow_select;
	int monthlast;
	int month_calculated = 0;
	
	char monthname[12][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	while (1)
	{
		localtime_r(&statmonth, &statmonth_tm);
		mvwprintw(win, 3, 2, "Display stats (Arrow keys for control)");
		wfill(win, 4, 1, 34, 58, " ");
		if(month_calculated == 0)
		{
			monthlast = monthlast_calculator(statmonth_tm.tm_mon + 1);
			arrow_select = monthlast + 1;
			month_calculated = 1;
		}
		mvwprintw(win, 5, 2, "Studytime of %s, %d", monthname[statmonth_tm.tm_mon + 1], statmonth_tm.tm_year + 1900);
		wprintw_quit(win, monthlast + 1, arrow_select);
		display_monthstats(win, statmonth_tm, arrow_select);
		
		c = wgetch(win);
		arrow_select = arrow_convert(c, arrow_select, monthlast + 1);
		if (c == '\n')
		{
			if(arrow_select == monthlast + 1)
				break;
			wfill(win, 4, 1, 34, 58, " ");
			display_daystats(win, statmonth_tm.tm_year + 1900, statmonth_tm.tm_mon + 1, arrow_select, 1);
		}
		if (c == ARROW_LEFT)
		{
			if (statmonth_tm.tm_mon > JANUARY) // 2월~12월
				statmonth_tm.tm_mon--;
			else // 1월
			{
				statmonth_tm.tm_year--;
				statmonth_tm.tm_mon = DECEMBER; // 12월
			}
			month_calculated = 0;
		}
		if (statmonth < today && c == ARROW_RIGHT)
		{
			if (statmonth_tm.tm_mon < DECEMBER) // 1월~11월
				statmonth_tm.tm_mon++;
			else // 12월
			{
				statmonth_tm.tm_year++;
				statmonth_tm.tm_mon = JANUARY;
			}
			month_calculated = 0;
		}
		statmonth = mktime(&statmonth_tm);
	}
	wclear(win);
}

void display_monthstats(WINDOW* win, struct tm statmonth_tm, int arrow_select)
{
	char UID_dir[256];
	sprintf(UID_dir, "%s/%s", ".", UID);

	if (chdir(UID_dir) == -1)
	{
		perror("chdir");
		exit(21);
	}

	timelog monthlog[32] = { 0, };
	int year, month, day;
	year = statmonth_tm.tm_year + 1900;
	month = statmonth_tm.tm_mon + 1;
	double total = 0.0;
	int monthlast;

	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		monthlast = 31;
		break;
	case 2:
		monthlast = 28;
		break;
	default:
		monthlast = 30;
	}

	for (int day_i = monthlast; day_i >= 1; day_i--)
	{
		char statfile[15];
		day = day_i;
		sprintf(statfile, "%04d%02d%02d.txt", year, month, day);

		monthlog[day_i].studytime = 0.0;
		int fd;
		if ((fd = open(statfile, O_RDONLY)) == -1)
			continue;

		timelog templog;
		while (read(fd, &templog, sizeof(timelog)) >= sizeof(timelog))
			monthlog[day_i].studytime += templog.studytime;
		total += monthlog[day_i].studytime;
		close(fd);
	}
	mvwprintw(win, 6, 2, "Total studytime: ");
	wsctotime(win, total);
	for (int day_i = 0; day_i < monthlast; day_i++)
	{
		char whatdate[12];
		sprintf(whatdate, "   %02d/%02d   ", month, day_i + 1);
		mvwprintw_standout(win, 8+(day_i/5)*4, 2+(day_i%5)*11, whatdate, day_i + 1, arrow_select);
		if(monthlog[day_i+1].studytime > 0)
			mvwprintw_standout(win, 9+(day_i/5)*4, 2+(day_i%5)*11, "     O     ", day_i + 1, arrow_select);
		else
			mvwprintw_standout(win, 9+(day_i/5)*4, 2+(day_i%5)*11, "     X     ", day_i + 1, arrow_select);
		wmove(win, 10+(day_i/5)*4, 2+(day_i%5)*11);
		wsctotime_month(win, monthlog[day_i+1].studytime);
	}

	if (chdir("..") == -1)
	{
		perror("chdir");
		exit(21);
	}
}
