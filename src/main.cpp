// Bais Hello World
//
// Compile: gcc -Wall -lncurses -o main main.c
// Run    : ./main

#include <iostream>
#include <ncurses.h>

#include <thread>
#include <chrono>

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
	/*while (!bExit) {

		cLayout.displayMiddle();

		nKey = cLayout.getchar();
		if (nKey == 27 || nKey == 'q') {
			// Exit the program
			bExit = true;
		}
	}*/

	cLayout.displayMiddle();
	cLayout.getchar();

	std::this_thread::sleep_for (std::chrono::seconds(1));

	std::vector<int> serialData;
	serialData.push_back(75);
	serialData.push_back(2);
	serialData.push_back(85);
	serialData.push_back(4);
	serialData.push_back(55);
	serialData.push_back(0);
	cLayout.processRoutingInformation(serialData);
	cLayout.displayMiddle();
	cLayout.getchar();

	std::this_thread::sleep_for (std::chrono::seconds(1));

	endwin();			// End curses mode

	return 0;
}
