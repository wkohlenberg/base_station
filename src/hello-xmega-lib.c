#include "hello-xmega-lib.h"

int _nTtyAcmConnection = 0;

bool InitXmegaSerial(char* pTtyAcm, int nBaudrate, int nParity) {

  struct termios oTty;

  // A possible string?
  if (pTtyAcm == NULL) {
		printf("No valid device name!\n");
		return false;
  }

  // Try to open the serial connection
  printf("[%s] ", pTtyAcm);
  _nTtyAcmConnection = open(pTtyAcm, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (_nTtyAcmConnection == 0)
	{
		perror(pTtyAcm);
		printf("Failed to open device: %s\n", pTtyAcm);
		return false;
	}

  // Clear struct to start with new settings
  memset(&oTty, 0, sizeof(oTty));
  if (tcgetattr(_nTtyAcmConnection, &oTty) != 0) {
    printf("Error %i getting settings: %s\n", errno, strerror(errno));
  }
  oTty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
  oTty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
  oTty.c_cflag |= CS8;            // 8 bits per byte (most common)
  oTty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
  oTty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  oTty.c_lflag &= ~ICANON;
  oTty.c_lflag &= ~ECHO;          // Disable echo
  oTty.c_lflag &= ~ECHOE;         // Disable erasure
  oTty.c_lflag &= ~ECHONL;        // Disable new-line echo
  oTty.c_lflag &= ~ISIG;          // Disable interpretation of INTR, QUIT and SUSP

  oTty.c_iflag |= IXON | IXOFF | IXANY; // Turn on s/w flow ctrl
  oTty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  oTty.c_oflag &= ~OPOST;         // Prevent special interpretation of output bytes (e.g. newline chars)
  oTty.c_oflag &= ~ONLCR;         // Prevent conversion of newline to carriage return/line feed

  oTty.c_cc[VTIME] = 1;           // Wait for up to 1s (1 deciseconds), returning as soon as any data is received.
  oTty.c_cc[VMIN] = 0;            // Minimal 0 chars read

  cfsetispeed(&oTty, B115200);    //nBaudrate);
  cfsetospeed(&oTty, B115200);    //nBaudrate);

  // Clear buffer and set new settings
  tcflush(_nTtyAcmConnection, TCIFLUSH);
  if (tcsetattr(_nTtyAcmConnection, TCSANOW, &oTty) != 0) {
    printf("Error %i setting settings: %s\n", errno, strerror(errno));
  };

  return true;
};

bool XmegaReadByte(uint8_t* nByte) {

  ssize_t nSize;

  // Check for valid open connection
  if (_nTtyAcmConnection == 0) {
    return false;
  }

  // Read one byte only
  nSize = read(_nTtyAcmConnection, nByte, 1);
  if (nSize == -1 && errno == 11) {
    return false;
  }
  if (nSize == 0) {
    return false;
  }

  if (nSize == -1) {
    printf("Error reading: %s", strerror(errno));
    return false;
  }

  return true;
};

bool CloseXmegaSerial() {

  if (_nTtyAcmConnection != 0) {
    close(_nTtyAcmConnection);
  }

  return true;
};
