// Bais Hello World
//
// Compile: gcc -Wall -lncurses -o main main.c
// Run    : ./main

#include <iostream>
#include <ncurses.h>

#include "layout.h"
#include "protocol.h"
#include "hello-xmega-lib.h"
#include "libtouch.h"

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

	// Start to search for the correct event-stream
	int nRet = LinuxInput_InitTouch();
	if (nRet < 0) {
		//printf("RaspberryPi 7\" Touch display is not found!\nError %d\n\n", nRet);
		std::cout << "RaspberryPi 7\" Touch display is not found!" << std::endl;
		std::cout << "Error " << nRet << std::endl << std::endl;
		return -2;
	}

	// Cheange touch settings
	_oLinuxInput_Settings.bRestartApply = false;
	_oLinuxInput_Settings.nRestartWait = 3;
	_oLinuxInput_Settings.bShutdownApply = false;
	_oLinuxInput_Settings.nShutdownWait = 3;

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

		// Update touch
	    LinuxInput_UpdateTouch();

	    // Check for restart and shutdown
	    if (_oLinuxInput_Settings.bRestartDetected) {
			LinuxInput_CloseTouch();
			endwin();

			LinuxInput_ApplyRestart();
			return 1;
	    }
	    if (_oLinuxInput_Settings.bShutdownDetected) {
			LinuxInput_CloseTouch();
			endwin();

			LinuxInput_ApplyShutdown();
			return 2;
	    }

		processSerialCommunication(cLayout);

		cLayout.displayTitle();
		cLayout.displayMiddle();

		nKey = cLayout.getchar();
		if (nKey == 27 || nKey == 'q') {
			// Exit the program
			bExit = true;
		}
	}

	// Close the device
	nRet = LinuxInput_CloseTouch();
	if (nRet < 0) {
		//printw("Close error %d!\n", nRet);
		std::cout << "Close error " << nRet << "!" << std::endl;
	}

	endwin();			// End curses mode
	CloseXmegaSerial();

	return 0;
}
