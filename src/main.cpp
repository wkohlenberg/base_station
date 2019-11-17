// Bais Hello World
//
// Compile: gcc -Wall -lncurses -o main main.c
// Run    : ./main

#include <iostream>
#include <ncurses.h>

#include "layout.h"

int main()
{
	initscr();			// Start curses mode
	clear();			// Clear the screen
	noecho();
	cbreak();
	curs_set(0);		// Hide cursor

	// Start colors on screen
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);

	CLayout cLayout;

	bool bExit = false;
	int nKey;
	while (!bExit) {

		cLayout.displayMiddle();

		nKey = cLayout.getchar();
		if (nKey == 27 || nKey == 'q') {
			// Exit the program
			bExit = true;
		}
	}

	endwin();			// End curses mode

	return 0;
}
