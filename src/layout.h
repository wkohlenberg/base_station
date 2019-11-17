#ifndef __LAYOUT_H_
#define __LAYOUT_H_

#include <iostream>
#include <vector>
#include <ncurses.h>
#include <string>

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

	std::vector<WINDOW *> m_winSensorData;
};

class CRoutingWindow{
public:
	CRoutingWindow();
	~CRoutingWindow();

	void updateRoutingWindow();

	void processRoutingInformation(std::vector<int> data);

	void addToRoutingTable(int dest, int hop);
	void deleteFromRoutingTable(int dest);

	int getchar();

	WINDOW *pRoutingWindow;

private:
	std::vector<routeInfo> vRoutingTable;
};

class CLayout{
public:
	CLayout();
	~CLayout();

	void displayTitle(unsigned page);
	void displayFooter();
	void displayMiddle(unsigned page);
	int getchar();

	void processRoutingInformation(std::vector<int> data);

	CSensorWindow cSensorWindow;
	CRoutingWindow cRoutingWindow;

	unsigned page = 1;						// Sensor is 0 and routing is 1

private:
	WINDOW *pTitleWindow;
	WINDOW *pFooterWindow;

	WINDOW *pSensorPageButton;
	WINDOW *pRoutingPageButton;

	int update = 0;

	// Title strings
	std::string sTitleSensorPage 	= "Sensor Data";
	std::string sTitleRoutingPage 	= "Routing Information";
};

#endif
