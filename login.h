#include <curses.h>
extern int usersFd;

void exist_IDerror(int argc, char* argv[]);
int ID_check(int argc, char* argv[]);
void login(int argc, char* argv[]);
WINDOW* initial_set();
void unsetup();
void set_forFirstRun();
