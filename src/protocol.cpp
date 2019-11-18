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
		result.push_back(std::stoi(substr));
	}

	return result;
}

// Process serial communication and push data to the correct window
int processSerialCommunication(CLayout &layout){

	uint8_t nType;
	int nPacket = 0;
	char aBuf[SERIAL_BUF_SIZE];

	if (XmegaReadByte(&nType)) {

		nPacket = ReceivePacket(aBuf, SERIAL_BUF_SIZE);
		std::vector<int> data = csvString(aBuf, ',');

		switch (nType) {
			case '!': // Type 1
				//printf("! [%i] [%s]\n", nPacket, aBuf);
				/*std::cout << "! [" << nPacket << "] ";
				for (unsigned i = 0; i < data.size(); i++) {
					std::cout << data.at(i) << " ";
				}
				std::cout << std::endl;*/
				layout.processRoutingInformation(data);
				break;

			case '@': // Type 2
				printf("@ [%i] [%s]\n", nPacket, aBuf);
				break;

			default:
				break;
		}
	}

	return 0;
}
