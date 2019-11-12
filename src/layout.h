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

	WINDOW *pRoutingWindow;
};

class CLayout{
public:
	CLayout();
	~CLayout();

	void displayTitle(unsigned page);
	void displayFooter();
	int getchar();

	CSensorWindow cSensorWindow;
	CRoutingWindow cRoutingWindow;

private:
	WINDOW *pTitleWindow;
	WINDOW *pFooterWindow;

	WINDOW *pSensorPageButton;
	WINDOW *pRoutingPageButton;

	// Title strings
	std::string sTitleSensorPage 	= "Sensor Data";
	std::string sTitleRoutingPage 	= "Routing Information";
};

#endif
