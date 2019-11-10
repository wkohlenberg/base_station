#include <iostream>

#include "layout.h"

/**
 * Class Layout
 */
CLayout::CLayout(){

	// Print page title
	pTitleWindow = newwin(1, COLS, 0, 0);
	displayTitle(1);

	cSensorWindow.displaySensorDataWindows();

	// Init footer
	pFooterWindow = newwin(4, COLS, 26, 0);
	displayFooter();
}

CLayout::~CLayout(){

}

void CLayout::displayTitle(unsigned page){
	wclear(pTitleWindow);

	int iStartX = 0;

	wattron(pTitleWindow, COLOR_PAIR(1));
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

	wattroff(pTitleWindow, COLOR_PAIR(1));
	wrefresh(pTitleWindow);
}

void CLayout::displayFooter(){
	box(pFooterWindow, 0, 0);
	wrefresh(pFooterWindow);
}

int CLayout::getchar(){
	return wgetch(pFooterWindow);
}

/**
 * Class Layout
 */
CSensorWindow::CSensorWindow(){
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 1));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 30));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 59));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 1));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 30));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 59));
}

CSensorWindow::~CSensorWindow(){
	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		delwin(m_winSensorData[i]);
	}
}

void CSensorWindow::updateSensorDataWindow(int nWin){
		wborder(m_winSensorData[nWin], '|', '|', '-', '-', '+', '+', '+', '+');
		mvwprintw(m_winSensorData[nWin], 1, 1, "Sensor ...");
		wrefresh(m_winSensorData[nWin]);
}

void CSensorWindow::displaySensorDataWindows(){
	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		updateSensorDataWindow(i);
	}
}
