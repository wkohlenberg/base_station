#include <iostream>

#include "layout.h"

CLayout::CLayout(){

	// Print page title
	pTitleWindow = newwin(1, COLS, 0, 0);
	displayTitle(1);

	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 1));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 30));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 59));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 1));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 30));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 59));

	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		wborder(m_winSensorData[i], '|', '|', '-', '-', '+', '+', '+', '+');
		mvwprintw(m_winSensorData[i], 1, 1, "Sensor ...");
		wrefresh(m_winSensorData[i]);
	}

	// Init footer
	pFooterWindow = newwin(4, COLS, 26, 0);
	box(pFooterWindow, 0, 0);
	wrefresh(pFooterWindow);
}

CLayout::~CLayout(){

	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		delwin(m_winSensorData[i]);
	}
}


void CLayout::displayTitle(unsigned page){
	wclear(pTitleWindow);

	int iStartX = 0;

	attron(COLOR_PAIR(2));
	switch (page){
		case 0:				// Menu
			break;
		case 1:
			iStartX = (COLS - sTitleSensorPage.length())/2;
			mvwprintw(pTitleWindow, 0, iStartX, "%s", sTitleSensorPage.c_str());
			break;
		case 2:
			iStartX = (COLS - sTitleRoutingPage.length())/2;
			mvwprintw(pTitleWindow, 0, iStartX, "%s", sTitleRoutingPage.c_str());
			break;
		default:
			break;
	}

	attroff(COLOR_PAIR(2));
	wrefresh(pTitleWindow);
}
