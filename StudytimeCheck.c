#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

int main()
{
	initscr();
	clear();
	move(10, 10);
	addstr("Welcome");
	refresh();
	sleep(5);
	endwin();
	
	return 0;
}
