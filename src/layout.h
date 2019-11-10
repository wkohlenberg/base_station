#ifndef __LAYOUT_H_
#define __LAYOUT_H_

#include <iostream>
#include <vector>
#include <ncurses.h>
#include <string>

#define SENSOR_DATA_WINDOWS			6
#define SENSOR_DATA_WINDOW_WIDTH	30
#define SENSOR_DATA_WINDOW_HEIGHT	13

class CSensorWindow{
public:
	CSensorWindow();
	~CSensorWindow();

	void updateSensorDataWindow(int nWin);
	void displaySensorDataWindows();

	std::vector<WINDOW*> m_winSensorData;
};

class CLayout{
public:
	CLayout();
	~CLayout();

	void displayTitle(unsigned page);
	void displayFooter();
	int getchar();

	CSensorWindow cSensorWindow;

private:
	WINDOW *pTitleWindow;
	WINDOW *pFooterWindow;

	// Title strings
	std::string sTitleSensorPage 	= "Sensor Data";
	std::string sTitleRoutingPage 	= "Routing Information";
};

#endif
