// Linux event system: find the RPi 7" Touchscreen
//
// Compile: gcc -Wall -lncurses -o find-touch find-touch.c
// Run:     ./find-touch
//
// Docu:
//  https://www.linuxjournal.com/article/6429
//  https://www.kernel.org/doc/html/v4.17/input/input_uapi.html
//  https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h

#include "libtouch.h"

// Global status variables
LinuxInput_Settings_t _oLinuxInput_Settings;
LinuxInput_MultiTouchSlot_t _aLinuxInput_Slot[LINUXINPUT_DEVICE_SLOT_COUNT];
LinuxInput_Touch_t _oLinuxInput_Touched;
struct winsize _oLinuxInput_TerminalSize;
bool _oLinuxInput_TerminalUsed;
int _nLinuxInput_EventId;
int _nLinuxInput_EventStream;
int _nLinuxInput_EventStream_CurSlot;

/** -----------------------------------
 *
 */
int LinuxInput_InitTouch() {

  DIR *pDir;
  struct dirent *pDirItem;
  int nDevice;
  char sFilename[1025];
  char sDeviceName[256];
  uint8_t aRestartSwipe[] = {0, 1, 2, 5, 8};
  uint8_t aShutdownSwipe[] = {0, 3, 6, 7, 8};

  // Init lib defaults for restart
  _oLinuxInput_Settings.bRestartApply = false;
  _oLinuxInput_Settings.nRestartWait = 5;
  _oLinuxInput_Settings.bRestartDetected = false;
  _oLinuxInput_Settings.nRestartSwipeSize = 5;
  memcpy(_oLinuxInput_Settings.aRestartSwipe, aRestartSwipe, _oLinuxInput_Settings.nRestartSwipeSize);
  _oLinuxInput_Settings.nRestartSwipeState = 0;

  // Init lib defaults for shutdown
  _oLinuxInput_Settings.bShutdownApply = true;
  _oLinuxInput_Settings.nShutdownWait = 5;
  _oLinuxInput_Settings.bShutdownDetected = false;
  _oLinuxInput_Settings.nShutdownSwipeSize = 5;
  memcpy(_oLinuxInput_Settings.aShutdownSwipe, aShutdownSwipe, _oLinuxInput_Settings.nShutdownSwipeSize);
  _oLinuxInput_Settings.nShutdownSwipeState = 0;

  // Init global vars
  memset(_aLinuxInput_Slot, 0, sizeof(LinuxInput_MultiTouchSlot_t) * LINUXINPUT_DEVICE_SLOT_COUNT);
  memset(&_oLinuxInput_Touched, 0, sizeof(LinuxInput_Touch_t));
  _nLinuxInput_EventId = -1;
  _nLinuxInput_EventStream = -1;
  _nLinuxInput_EventStream_CurSlot = 0;

  // Try to open the Linux Input directory
  pDir = opendir("/dev/input");
  if (pDir == NULL) {
    //printf("Error: could not open /dev/input\n\n");
    return -2;
  }

  // Check all "event#"
  _nLinuxInput_EventId = 0;
  while ((pDirItem = readdir(pDir)) != NULL) {

    // Parse directory item for: event#
    if (sscanf(pDirItem->d_name, "event%u", &_nLinuxInput_EventId) == 1) {
      //printf("%d %s\n", nNr, pDirItem->d_name);

      // Get the device information
      sprintf(sFilename, "/dev/input/%s", pDirItem->d_name);
      nDevice = open(sFilename, O_RDONLY | O_NONBLOCK);
      if (nDevice <= 0) {
        //printf("Error: unable to open event-stream\n\n");
        return -3;
      }
      else {
        // Event-stream is open
        if (ioctl(nDevice, EVIOCGNAME(256), sDeviceName) >= 0) {
          //printf("Device [%s] with name [%s]\n", pDirItem->d_name, sDeviceName);

          if (strcmp(sDeviceName, LINUXINPUT_DEVICE_NAME) == 0) {
            // Found :-) so no closeing needed
            _nLinuxInput_EventStream = nDevice;

            // Return the event number
            return _nLinuxInput_EventId;
          }
        }

        // Close event-stream
        close(nDevice);
      }
    }
  }

  // Close directory read
  closedir(pDir);

  // None found
  //printf("Error: could not find the touchscreen\n\n");
  return -1;
}

/** -----------------------------------
 *
 */
bool LinuxInput_UpdateTouch() {

  bool bUpdate = false;
  struct input_event oEvent;
  ssize_t nReadCount;

  // A valid event-stream?
  if (_nLinuxInput_EventStream < 0) {
    return false;
  }

  // Determine terminal size in chr and pix
  ioctl(0, TIOCGWINSZ, &_oLinuxInput_TerminalSize);
  if (_oLinuxInput_TerminalSize.ws_xpixel == 0 && _oLinuxInput_TerminalSize.ws_ypixel == 0) {
    _oLinuxInput_TerminalUsed = false;
    _oLinuxInput_TerminalSize.ws_xpixel = LINUXINPUT_DEVICE_WIDTH_PIX;
    _oLinuxInput_TerminalSize.ws_ypixel = LINUXINPUT_DEVICE_HEIGHT_PIX;
  }
  else {
    _oLinuxInput_TerminalUsed = true;
  }

  // Any messages to read? error 11 EAGAIN = Try again (= no data yet because of no-blocking)
  while (!(nReadCount = read(_nLinuxInput_EventStream, &oEvent, sizeof(struct input_event)) == -1 && errno == 11)) {
    switch (oEvent.type) {
        case EV_SYN: // SYN_REPORT
          bUpdate = true;
          break;

        case EV_KEY:
          switch (oEvent.code) {
            case BTN_TOUCH:
              if (oEvent.value > 0) {
                // Start touching
                _oLinuxInput_Touched.bButton = true;
              }
              else {
                // No toyching
                _oLinuxInput_Touched.bButton = false;

                // Clear all touch data
                for (int i = 0; i < LINUXINPUT_DEVICE_SLOT_COUNT; i++) {
                  _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].bUsed = false;
                  _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nId = 0;
                }
              }
              bUpdate = true;
              break;
          }
          break;

        case EV_REL:
          break;

        case EV_ABS:
          switch (oEvent.code) {
            case ABS_X:
              _oLinuxInput_Touched.nX = oEvent.value;
              _oLinuxInput_Touched.nCol = (_oLinuxInput_Touched.nX * _oLinuxInput_TerminalSize.ws_col) / _oLinuxInput_TerminalSize.ws_xpixel;
              bUpdate = true;
              break;

            case ABS_Y:
              _oLinuxInput_Touched.nY = oEvent.value;
              _oLinuxInput_Touched.nRow = (_oLinuxInput_Touched.nY * _oLinuxInput_TerminalSize.ws_row) / _oLinuxInput_TerminalSize.ws_ypixel;
              bUpdate = true;
              break;

            case ABS_MT_SLOT:
              _nLinuxInput_EventStream_CurSlot = oEvent.value;
              bUpdate = true;
              break;

            case ABS_MT_TRACKING_ID:
              if (oEvent.value == 0xffffffff) {
                _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].bUsed = false;
                _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nId = 0;
              }
              else {
                _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].bUsed = true;
                _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nId = oEvent.value;
              }
              bUpdate = true;
              break;

            case ABS_MT_POSITION_X:
              _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].bUsed = true;
              _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nX = oEvent.value;
              _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nCol = (_aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nX * _oLinuxInput_TerminalSize.ws_col) / _oLinuxInput_TerminalSize.ws_xpixel;
              bUpdate = true;
              LinuxInput_UpdateSwipe();
              break;

            case ABS_MT_POSITION_Y:
              _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].bUsed = true;
              _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nY = oEvent.value;
              _aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nRow = (_aLinuxInput_Slot[_nLinuxInput_EventStream_CurSlot].nY * _oLinuxInput_TerminalSize.ws_row) / _oLinuxInput_TerminalSize.ws_ypixel;
              bUpdate = true;
              LinuxInput_UpdateSwipe();
              break;
          }
          break;
    }
  }

  return bUpdate;
}

/** -----------------------------------
 *
 */
int LinuxInput_CloseTouch() {

  return close(_nLinuxInput_EventStream);
}

/** -----------------------------------
 *
 */
void LinuxInput_ApplyRestart() {

  // NO clear screen and start applying after countdown
  printf("\n\nApply restart ... "); fflush(stdout);
  for (uint8_t t = _oLinuxInput_Settings.nRestartWait; t > 0; t--) {
    printf("%d ", t); fflush(stdout);
    sleep(1);
  }
  printf("now!\n\n"); fflush(stdout);
  system("/home/pi/base_station/src/libtouch-scripts/do_restart.sh");

  exit(1);
}

/** -----------------------------------
 *
 */
void LinuxInput_ApplyShutdown() {

  // NO clear screen and start applying after countdown
  printf("\n\nApply shutdown ... "); fflush(stdout);
  for (uint8_t t = _oLinuxInput_Settings.nShutdownWait; t > 0; t--) {
    printf("%d ", t); fflush(stdout);
    sleep(1);
  }
  printf("now!\n\n"); fflush(stdout);
  system("/home/pi/base_station/src/libtouch-scripts/do_shutdown.sh");

  exit(2);
}

/** -----------------------------------
 *
 */
void LinuxInput_UpdateSwipe() {

  uint16_t nWidth, nZoneWidth;
  uint16_t nHeight, nZoneHeight;
  uint16_t nCurrentX, nCurrentY;
  uint16_t nZoneNr;

  // LINUXINPUT_SWIPE_WIDTH_COUNT 3
  // LINUXINPUT_SWIPE_HEIGHT_COUNT 3
  //
  // Swipezones:  0 1 2
  //              3 4 5
  //              6 7 8
  // A swipe:
  // - Start slot 0 holding in zone 0
  // - Move route in slot 1 over multiple zones

  nWidth       = LINUXINPUT_DEVICE_WIDTH_PIX; // Not: _oLinuxInput_TerminalSize.ws_xpixel;
  nZoneWidth   = nWidth / LINUXINPUT_SWIPE_WIDTH_COUNT;
  nHeight      = LINUXINPUT_DEVICE_HEIGHT_PIX; // Not: _oLinuxInput_TerminalSize.ws_ypixel;
  nZoneHeight  = nHeight / LINUXINPUT_SWIPE_HEIGHT_COUNT;

  // Two slots required
  if (LINUXINPUT_DEVICE_SLOT_COUNT < 2) {
    return;
  }

  // Active slot 0 in zone 1?
  if (!_aLinuxInput_Slot[0].bUsed || _aLinuxInput_Slot[0].nX > nZoneWidth || _aLinuxInput_Slot[0].nY > nZoneHeight) {
    return;
  }

  // Active slot 1?
  if (!_aLinuxInput_Slot[1].bUsed) {

    // Reset the swipes
    _oLinuxInput_Settings.nRestartSwipeState = 0;
    _oLinuxInput_Settings.nShutdownSwipeState = 0;
    return;
  }

  // Use curremt x,y that are always from the touchscreen (not from the terminal) to calculate the zone
  nCurrentX = _aLinuxInput_Slot[1].nX;
  nCurrentY = _aLinuxInput_Slot[1].nY;
  nZoneNr = (nCurrentY / nZoneHeight) * LINUXINPUT_SWIPE_WIDTH_COUNT + (nCurrentX / nZoneWidth);

  //mvprintw(8,45,"wxh %d x %d  T(%d x %d)  zone wxh %d x %d        ", nWidth, nHeight, _oLinuxInput_TerminalSize.ws_xpixel, _oLinuxInput_TerminalSize.ws_ypixel, nZoneWidth, nZoneHeight);
  //mvprintw(10,45,"zone xy %d,%d   xy %d %d   = %3d ", nCurrentX, nCurrentY, (nCurrentX / nZoneWidth), (nCurrentY / nZoneHeight), nZoneNr);
  //refresh();

  // Update the swipe?
  if (_oLinuxInput_Settings.aRestartSwipe[_oLinuxInput_Settings.nRestartSwipeState] == nZoneNr) {
    // Still in the current swipe zone; no action needed
  }
  else if (_oLinuxInput_Settings.aRestartSwipe[_oLinuxInput_Settings.nRestartSwipeState + 1] == nZoneNr) {
    // Go to next swipe state
    _oLinuxInput_Settings.nRestartSwipeState++;

    // It is the last swipe state?
    if (_oLinuxInput_Settings.nRestartSwipeState + 1 == _oLinuxInput_Settings.nRestartSwipeSize) {

      _oLinuxInput_Settings.nRestartSwipeState = 0;
      _oLinuxInput_Settings.bRestartDetected = true;

      // Apply?
      if (_oLinuxInput_Settings.bRestartApply) {
        LinuxInput_ApplyRestart();
      }
    }
  }
  else {
    // Wrong swipe zones
    _oLinuxInput_Settings.nRestartSwipeState = 0;
  }
  //mvprintw(12,45,"restart state %d of %d ", _oLinuxInput_Settings.nRestartSwipeState, _oLinuxInput_Settings.nRestartSwipeSize);
  //refresh();

  // Update the swipe?
  if (_oLinuxInput_Settings.aShutdownSwipe[_oLinuxInput_Settings.nShutdownSwipeState] == nZoneNr) {
    // Still in the current swipe zone; no action needed
  }
  else if (_oLinuxInput_Settings.aShutdownSwipe[_oLinuxInput_Settings.nShutdownSwipeState + 1] == nZoneNr) {
    // Go to next swipe state
    _oLinuxInput_Settings.nShutdownSwipeState++;

    // It is the last swipe state?
    if (_oLinuxInput_Settings.nShutdownSwipeState + 1 == _oLinuxInput_Settings.nShutdownSwipeSize) {

      _oLinuxInput_Settings.nShutdownSwipeState = 0;
      _oLinuxInput_Settings.bShutdownDetected = true;

      // Apply?
      if (_oLinuxInput_Settings.bShutdownApply) {
        LinuxInput_ApplyShutdown();
      }
    }
  }
  else {
    // Wrong swipe zones
    _oLinuxInput_Settings.nShutdownSwipeState = 0;
  }
  //mvprintw(12,45,"Shutdown state %d of %d ", _oLinuxInput_Settings.nShutdownSwipeState, _oLinuxInput_Settings.nShutdownSwipeSize);
  //refresh();
}
