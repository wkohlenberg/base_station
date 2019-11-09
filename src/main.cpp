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

	CLayout cLayout;

	wgetch(cLayout.m_winSensorData[0]);



	endwin();			// End curses mode

	return 0;
}
