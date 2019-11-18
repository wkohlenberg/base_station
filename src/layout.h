#ifndef __LAYOUT_H_
#define __LAYOUT_H_

#include <iostream>
#include <vector>
#include <ncurses.h>
#include <string>
#include <ctime>

#define SENSOR_DATA_WINDOWS			6
#define SENSOR_DATA_WINDOW_WIDTH	30
#define SENSOR_DATA_WINDOW_HEIGHT	13

#define ROUTING_WINDOW_WIDTH		50
#define ROUTING_WINDOW_HEIGHT		24

struct routeInfo{
	int hops;
	int destination;
};

class CSensorWindow{
public:
	CSensorWindow();
	~CSensorWindow();

	void updateSensorDataWindow(int nWin);
	void displaySensorDataWindows();

	std::vector<WINDOW*> m_winSensorData;
};

class CRoutingWindow{
public:
	CRoutingWindow();
	~CRoutingWindow();

	void updateRoutingWindow();

	void addToRoutingTable(int dest, int hop);
	void deleteFromRoutingTable(int dest);
	void processRoutingInformation(std::vector<int> data);
	int getchar();

	WINDOW *pRoutingWindow;

private:
	std::vector<routeInfo> vRoutingTable;

	int cnt;
};

class CLayout{
public:
	CLayout();
	~CLayout();

	void displayTitle(unsigned page);
	void displayMiddle();
	void displayFooter();
	int getchar();
	void processRoutingInformation(std::vector<int> data);

	CSensorWindow cSensorWindow;
	CRoutingWindow cRoutingWindow;

private:
	WINDOW *pTitleWindow;
	WINDOW *pFooterWindow;

	WINDOW *pSensorPageButton;
	WINDOW *pRoutingPageButton;

	tm *locTime;

	// Title strings
	std::string sTitleSensorPage 	= "Sensor Data";
	std::string sTitleRoutingPage 	= "Routing Information";
};

#endif
