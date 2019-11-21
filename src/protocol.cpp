#include <sstream>

#include "protocol.h"
#include "hello-xmega-lib.h"

int ReceivePacket(char* aBuf, int nSize) {

	uint8_t nPacket, nByte;
	memset(aBuf, 0, nSize);				// Empty the buffer
	while (!XmegaReadByte(&nPacket));	// Read the packet size (blocking style)

	// Read each byte (blocking style)
	for (uint i = 0; i < nPacket; i++) {
		while (!XmegaReadByte(&nByte));
		aBuf[i] = (char)nByte;
	}

	return (int)nPacket;
}

// Parse csv strings from serial communication
std::vector<int> csvString(std::string dataString, char delim){

	std::vector<int> result;
	std::stringstream sstream(dataString);
	while(sstream.good()){
		std::string substr;
		getline(sstream, substr, ',');
		try{
			result.push_back(std::stoi(substr));
		}
		catch (const std::invalid_argument& ia){}
		catch (const std::out_of_range& oof){}
	}

	return result;
}

// Process serial communication and push data to the correct window
int processSerialCommunication(CLayout &layout){

	uint8_t nType;
	char aBuf[SERIAL_BUF_SIZE];

	if (XmegaReadByte(&nType)) {

		ReceivePacket(aBuf, SERIAL_BUF_SIZE);
		std::vector<int> data = csvString(aBuf, ',');

		if (!data.empty()){
			switch (nType) {
				case '!': // Type 1
					layout.processRoutingInformation(data);
					break;

				case '@': // Type 2
					layout.processSensorInformation(data);
					break;

				default:
					break;
			}
		}
	}

	return 0;
}
