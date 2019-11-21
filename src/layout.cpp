#include <iostream>

#include "layout.h"

/**
 * Class Sensor Window
 */
CSensorWindow::CSensorWindow(){
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 6));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 35));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 1, 64));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 6));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 35));
	m_winSensorData.push_back(newwin(SENSOR_DATA_WINDOW_HEIGHT, SENSOR_DATA_WINDOW_WIDTH, 13, 64));
}

CSensorWindow::~CSensorWindow(){
	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		delwin(m_winSensorData[i]);
	}
}

void CSensorWindow::updateSensorDataWindow(int nWin){
		wborder(m_winSensorData[nWin], '|', '|', '-', '-', '+', '+', '+', '+');

		// Update sensor Information if data is available for this window
		if ((unsigned)nWin < vSensorInfo.size()){
			mvwprintw(m_winSensorData[nWin], 1, 2, "Sensor %d:          Value", vSensorInfo[nWin].ID);

			int row = 2;
			for (unsigned iData = vSensorInfo[nWin].data.size(); iData > 0; iData--){
				mvwprintw(m_winSensorData[nWin], row++, 2, "%s              %d", vSensorInfo[nWin].data[(iData-1)].timestamp.c_str(), vSensorInfo[nWin].data[(iData-1)].value);
			}
		}
		wrefresh(m_winSensorData[nWin]);
}

void CSensorWindow::displaySensorDataWindows(){
	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		updateSensorDataWindow(i);
	}
}

void CSensorWindow::processSensorInformation(std::vector<int> data){

	// put msb and lsb together
	int iValue = data[2] + (data[1] << 8);
	int ID = data[0];

	// Check if ID already exists
	for (unsigned iInfo = 0; iInfo < vSensorInfo.size(); iInfo++){
		if (ID == vSensorInfo[iInfo].ID){
			vSensorInfo[iInfo].data.push_back(sensorData());
			vSensorInfo[iInfo].data.back().value = iValue;

			// Add time component
			time_t rawtime;
			time(&rawtime);
			struct tm* timeinfo = localtime(&rawtime);
			char buf[20];
			strftime (buf,sizeof(buf),"%T",timeinfo);
			vSensorInfo[iInfo].data.back().timestamp = buf;

			// Check if data length is longer than ten and remove the extra values
			unsigned dataLength = vSensorInfo[iInfo].data.size();
			if (dataLength > 10){
				for (unsigned i = 0; i < (dataLength-10); i++){
					vSensorInfo[iInfo].data.erase(vSensorInfo[iInfo].data.begin());
				}
			}

			return;
		}
	}

	// ID do not exist and check if there is a window left
	if (vSensorInfo.size() < 6){
		vSensorInfo.push_back(sensorInfo());
		vSensorInfo.back().ID = ID;

		vSensorInfo.back().data.push_back(sensorData());
		vSensorInfo.back().data.back().value = iValue;

		// Add time component
		time_t rawtime;
		time(&rawtime);
		struct tm* timeinfo = localtime(&rawtime);
		char buf[20];
		strftime (buf,sizeof(buf),"%T",timeinfo);
		vSensorInfo.back().data.back().timestamp = buf;
	}
}

void CSensorWindow::clearWindow(){
	for (unsigned i = 0; i < SENSOR_DATA_WINDOWS; i++){
		wclear(m_winSensorData[i]);
		wrefresh(m_winSensorData[i]);
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
}

CRoutingWindow::~CRoutingWindow(){
	delwin(pRoutingWindow);
}

void CRoutingWindow::updateRoutingWindow(){
	box(pRoutingWindow, 0, 0);

	mvwprintw(pRoutingWindow, 1, 1, "Time       Destination    hops");
	for (unsigned i = 0; i < vRoutingTable.size(); i++){
		mvwprintw(pRoutingWindow, (i+2), 1, "%s            %2d      %2d", vRoutingTable[i].timestamp.c_str(), vRoutingTable[i].destination, vRoutingTable[i].hops);
	}

	// Print empty rows to clear screen
	for (unsigned i = vRoutingTable.size(); i < ROUTING_WINDOW_HEIGHT-3; i++){
		mvwprintw(pRoutingWindow, (i+2), 1, "                                ");
	}

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

	// Add time component
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	char buf[20];
	strftime (buf,sizeof(buf),"%T",timeinfo);
	vRoutingTable.back().timestamp = buf;
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
		if (dataIndex < data.size()){
			hop = data.at(dataIndex);

			if (hop == 0){
				deleteFromRoutingTable(dest);
			}
			else if (hop > 0){
				addToRoutingTable(dest, hop);
			}
		}
		else {return;}
	}
}

int CRoutingWindow::getchar(){
	return wgetch(pRoutingWindow);
}

void CRoutingWindow::clearWindow(){
	wclear(pRoutingWindow);
	wrefresh(pRoutingWindow);
}

/**
 * Class Layout
 */
CLayout::CLayout(){

	// Get date time from ctime
	time_t now = time(0);
	locTime = localtime(&now);

	// Set default page
	setPageNumber(DEFAULT_PAGE_NUMBER);
	lastPage = DEFAULT_PAGE_NUMBER;

	// Print page title
	pTitleWindow = newwin(1, COLS, 0, 0);

	displayTitle();
	displayMiddle();

	// Init footer
	pFooterWindow = newwin(4, 82, 26, 0);
	pSensorPageButton = newwin(4, 9, 26, 82);
	pRoutingPageButton = newwin(4, 9, 26, 91);

	mvwprintw(pFooterWindow, 1, 1, "[ESC] to quit, LB>RB>RO to restart, LB>LO>RO to shutdown.");
	mvwprintw(pSensorPageButton, 1, 1, "Sensor");
	mvwprintw(pRoutingPageButton, 1, 2, "Route");
	displayFooter();
}

CLayout::~CLayout(){

}

void CLayout::displayTitle(){
	mvwprintw(pTitleWindow, 0, 90, "%d-%d-%d", locTime->tm_mday, locTime->tm_mon, (1900+locTime->tm_year));

	int iStartX = 30;

	wattron(pTitleWindow, COLOR_PAIR(1));
	switch (getPageNumber()){
		case 0:				// Menu
			break;
		case 1:
			mvwprintw(pTitleWindow, 0, iStartX, "                                          ");// clear title
			iStartX = (COLS - sTitleSensorPage.length())/2;
			mvwprintw(pTitleWindow, 0, iStartX, "%s", sTitleSensorPage.c_str());
			break;
		case 2:
			mvwprintw(pTitleWindow, 0, iStartX, "                                          ");// clear title
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

	switch (getPageNumber()){
		case 0:				// Menu
			break;
		case 1:
			if (getLastPageNumber() != getPageNumber()){
				cRoutingWindow.clearWindow();
			}
			cSensorWindow.displaySensorDataWindows();
			break;
		case 2:
			if (getLastPageNumber() != getPageNumber()){
				cSensorWindow.clearWindow();
			}
			cRoutingWindow.updateRoutingWindow();
			break;
		default:
			break;
	}
}

int CLayout::getchar(){
	return cRoutingWindow.getchar();
}

void CLayout::processRoutingInformation(std::vector<int> data){
	cRoutingWindow.processRoutingInformation(data);
}

void CLayout::processSensorInformation(std::vector<int> data){
	cSensorWindow.processSensorInformation(data);
}
