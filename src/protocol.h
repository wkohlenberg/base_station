#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>
#include "layout.h"


#define SERIAL_BUF_SIZE			255

int ReceivePacket(char* aBuf, int nSize);
int processSerialCommunication(CLayout &layout, std::ofstream& file);

#endif
