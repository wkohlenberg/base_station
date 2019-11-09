#ifndef __LAYOUT_H_
#define __LAYOUT_H_

#include <iostream>
#include <vector>
#include <ncurses.h>
#include <string>

#define SENSOR_DATA_WINDOWS			6
#define SENSOR_DATA_WINDOW_WIDTH	30
#define SENSOR_DATA_WINDOW_HEIGHT	13

class CLayout{
public:
	CLayout();
	~CLayout();

	void displayTitle(unsigned page);

	std::vector<WINDOW*> m_winSensorData;

private:
	WINDOW *pTitleWindow;
	WINDOW *pFooterWindow;

	// Title strings
	std::string sTitleSensorPage 	= "Sensor Data";
	std::string sTitleRoutingPage 	= "Routing Information";
};

#endif
