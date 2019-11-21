// Bais Hello World
//
// Compile: gcc -Wall -lncurses -o main main.c
// Run    : ./main

#include <iostream>
#include <ncurses.h>
#include <fstream>

#include "layout.h"
#include "protocol.h"
#include "hello-xmega-lib.h"
#include "libtouch.h"

int CheckButtonPressed() {

	if (_oLinuxInput_Touched.bButton) {
		// Sensor page button
		if (((_oLinuxInput_Touched.nCol >= 82) && (_oLinuxInput_Touched.nCol <= 91)) &&
			((_oLinuxInput_Touched.nRow >= 26) && (_oLinuxInput_Touched.nRow <= 30))) {
			return 1;
		}
		// Routing page button
		else if (((_oLinuxInput_Touched.nCol >= 91) && (_oLinuxInput_Touched.nCol <= 100)) &&
			((_oLinuxInput_Touched.nRow >= 26) && (_oLinuxInput_Touched.nRow <= 30))) {
			return 2;
		}
	}

	return 0;
}

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

	// Open stream
	std::ofstream logFile;
	logFile.open("debug.txt", std::ofstream::app);
	if (!logFile.is_open()){
		std::cout << "Could not open log file." << std::endl;
		return -3;
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

		// Check for buttons pressed
		if (CheckButtonPressed() == 1){
			cLayout.setPageNumber(1);
		}
		else if (CheckButtonPressed() == 2){
			cLayout.setPageNumber(2);
		}

		processSerialCommunication(cLayout, logFile);

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

	logFile.close();

	endwin();			// End curses mode
	CloseXmegaSerial();

	return 0;
}
