#include <iostream>

#include "layout.h"

/**
 * Class Sensor Window
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

/**
 * Class Routing Window
 */
CRoutingWindow::CRoutingWindow(){
	int iStartX = (COLS - ROUTING_WINDOW_WIDTH)/2;
	pRoutingWindow = newwin(ROUTING_WINDOW_HEIGHT, ROUTING_WINDOW_WIDTH, 2, iStartX);
	nodelay(pRoutingWindow, true);

	// Add some test data
	addToRoutingTable(55, 4);
	addToRoutingTable(65, 6);
	addToRoutingTable(81, 2);

	cnt = 0;
}

CRoutingWindow::~CRoutingWindow(){
	delwin(pRoutingWindow);
}

void CRoutingWindow::updateRoutingWindow(){
	box(pRoutingWindow, 0, 0);

	mvwprintw(pRoutingWindow, 1, 1, "Time       Destination    hops");
	for (unsigned i = 0; i < vRoutingTable.size(); i++){
		mvwprintw(pRoutingWindow, (i+2), 1, "hh:mm:ss            %2d      %2d", vRoutingTable[i].destination, vRoutingTable[i].hops);
	}

	mvwprintw(pRoutingWindow, 10, 1, "Update %i", cnt++);

	wrefresh(pRoutingWindow);
}

void CRoutingWindow::addToRoutingTable(int dest, int hop){

	for (unsigned rtIndex = 0; rtIndex < vRoutingTable.size(); rtIndex++){
		if (vRoutingTable[rtIndex].destination == dest){
			vRoutingTable[rtIndex].hops = hop;
			return;
		}
	}

	vRoutingTable.push_back(routeInfo());
	vRoutingTable.back().destination = dest;
	vRoutingTable.back().hops = hop;
}

void CRoutingWindow::deleteFromRoutingTable(int dest){

	unsigned rtIndex = 0;
	for (rtIndex = 0; rtIndex < vRoutingTable.size(); rtIndex++){
		if (vRoutingTable[rtIndex].destination == dest){
			vRoutingTable.erase(vRoutingTable.begin()+rtIndex);
			break;
		}
	}
}

void CRoutingWindow::processRoutingInformation(std::vector<int> data){
	int dest = 0;
	int hop = 0;

	for (unsigned dataIndex = 0; dataIndex < data.size(); dataIndex++){
		dest = data.at(dataIndex++);
		hop = data.at(dataIndex);

		if (hop == 0){
			deleteFromRoutingTable(dest);
		}
		else if (hop > 0){
			addToRoutingTable(dest, hop);
		}
	}
}

int CRoutingWindow::getchar(){
	return wgetch(pRoutingWindow);
}

/**
 * Class Layout
 */
CLayout::CLayout(){

	// Print page title
	pTitleWindow = newwin(1, COLS, 0, 0);
	//displayTitle(1);

	displayTitle(2);
	cRoutingWindow.updateRoutingWindow();
	//cSensorWindow.displaySensorDataWindows();

	// Init footer
	pFooterWindow = newwin(4, 82, 26, 0);
	pSensorPageButton = newwin(4, 9, 26, 82);
	pRoutingPageButton = newwin(4, 9, 26, 91);

	mvwprintw(pSensorPageButton, 1, 1, "Sensor");
	mvwprintw(pRoutingPageButton, 1, 2, "Route");
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

	box(pSensorPageButton, 0, 0);
	box(pRoutingPageButton, 0, 0);

	wrefresh(pFooterWindow);
	wrefresh(pSensorPageButton);
	wrefresh(pRoutingPageButton);
}

void CLayout::displayMiddle(){
	cRoutingWindow.updateRoutingWindow();
}

int CLayout::getchar(){
	//return wgetch(pFooterWindow);
	return cRoutingWindow.getchar();
}

void CLayout::processRoutingInformation(std::vector<int> data){
	cRoutingWindow.processRoutingInformation(data);
}
