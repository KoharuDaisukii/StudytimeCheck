#include <curses.h>
#define ARROW_DOWN 258
#define ARROW_UP 259
#define ARROW_LEFT 260
#define ARROW_RIGHT 261

int arrow_convert(int key_select, int arrow_select, int bound);

void wsctotime(WINDOW*, int seconds);
void wcleardel(WINDOW*);
void wdraw_timebar(WINDOW*, int y_start, int x_start);
void mvwprintw_standout(WINDOW*, int y, int x, char*, int line_num, int arrow_select);
void wprintw_standout(WINDOW*, char*, int line_num, int arrow_select);
void wfill(WINDOW*, int y1, int x1, int y2, int x2, char* ch);

int rmdir_r(char* path);



