// Bais Hello World
//
// Compile: gcc -Wall -lncurses -o main main.c
// Run    : ./main

#include <iostream>
#include <ncurses.h>

#include "layout.h"
#include "protocol.h"
#include "hello-xmega-lib.h"

int main(){

	// Init the correct Xmega-connection
	std::cout << "> Connecting: ";
	char sCommPath[] = "/dev/ttyACM0";
	if (!InitXmegaSerial(sCommPath, 115200, 0)) {
		std::cout << "error" << std::endl;
		return -1;
	}
	else {
		std::cout << "done" << std::endl;
	}

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

		processSerialCommunication(cLayout);

		cLayout.displayMiddle();

		nKey = cLayout.getchar();
		if (nKey == 27 || nKey == 'q') {
			// Exit the program
			bExit = true;
		}
	}

	endwin();			// End curses mode
	CloseXmegaSerial();

	return 0;
}
