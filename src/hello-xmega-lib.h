#ifndef _HELLO_XMEGA_LIB_H_
  #define _HELLO_XMEGA_LIB_H_

  #ifdef __cplusplus
  extern "C" {
  #endif

  #include <stdio.h>
  #include <string.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <termios.h>
  #include <unistd.h>

  #include <stdbool.h>
  #include <stdint.h>
  #include <stdlib.h>

  bool InitXmegaSerial(char* pTtyAcm, int nBaudrate, int nParity);
  bool XmegaReadByte(uint8_t* nByte);
  bool CloseXmegaSerial();

  #ifdef __cplusplus
  }
  #endif

#endif // _HELLO_XMEGA_LIB_H_
