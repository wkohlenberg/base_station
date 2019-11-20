// Linux event system: find the RPi 7" Touchscreen
//
// Docu:
//  https://www.linuxjournal.com/article/6429
//  https://www.kernel.org/doc/html/v4.17/input/input_uapi.html
//  https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h

#ifndef _LIBTOUCH_H_
  #define _LIBTOUCH_H_

  #ifdef __cplusplus
  extern "C" {
  #endif

  // System include files
  #include <stdio.h>
  #include <sys/types.h>
  #include <sys/ioctl.h>
  #include <dirent.h>
  #include <linux/input.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <errno.h>
  #include <string.h>
  #include <stdbool.h>
  #include <stdint.h>
  #include <stdlib.h>

  // Library defines
  #define LINUXINPUT_DEVICE_SLOT_COUNT 10
  #define LINUXINPUT_DEVICE_NAME "FT5406 memory based driver"
  #define LINUXINPUT_DEVICE_WIDTH_PIX 800
  #define LINUXINPUT_DEVICE_HEIGHT_PIX 480
  #define LINUXINPUT_SWIPE_WIDTH_COUNT 3
  #define LINUXINPUT_SWIPE_HEIGHT_COUNT 3

  // Helper structs
  struct LinuxInput_MultiTouchSlot {
    bool bUsed;       // false: not active used, true: active
    uint32_t nId;     // EV_ABS  ABS_MT_TRACKING_ID
    uint32_t nX;      // EV_ABS   ABS_MT_POSITION_X
    uint32_t nY;      // EV_ABS   ABS_MT_POSITION_Y
    uint32_t nCol;    // column
    uint32_t nRow;    // row
  };
  typedef struct LinuxInput_MultiTouchSlot LinuxInput_MultiTouchSlot_t;

  struct LinuxInput_Touch {
    bool bButton;     // false: no touch, true: touched
    uint32_t nX;      // EV_ABS               ABS_X
    uint32_t nY;      // EV_ABS               ABS_Y
    uint32_t nCol;    // column
    uint32_t nRow;    // row
  };
  typedef struct LinuxInput_Touch LinuxInput_Touch_t;

  struct LinuxInput_Settings {
    // Restart settings and status
    bool bRestartApply;           // Apply restart if detected, otherwise marking only
    uint8_t nRestartWait;         // Waiting time in seconds
    bool bRestartDetected;        // Restart detected status
    uint8_t aRestartSwipe[9];     // Swipe route
    uint8_t nRestartSwipeSize;    // Swipe route length
    uint8_t nRestartSwipeState;   // Swipe route state

    // Shutdown settings and status
    bool bShutdownApply;          // Apply shutdown if detected, otherwise marking only
    uint8_t nShutdownWait;        // Waiting time in seconds
    bool bShutdownDetected;       // Shutdown detected status
    uint8_t aShutdownSwipe[9];    // Swipe route
    uint8_t nShutdownSwipeSize;   // Swipe route length
    uint8_t nShutdownSwipeState;  // Swipe route state
  };
  typedef struct LinuxInput_Settings LinuxInput_Settings_t;

  // Global status variables
  extern LinuxInput_Settings_t _oLinuxInput_Settings;
  extern LinuxInput_MultiTouchSlot_t _aLinuxInput_Slot[LINUXINPUT_DEVICE_SLOT_COUNT];
  extern LinuxInput_Touch_t _oLinuxInput_Touched;
  extern struct winsize _oLinuxInput_TerminalSize;
  extern bool _oLinuxInput_TerminalUsed;
  extern int _nLinuxInput_EventId;
  extern int _nLinuxInput_EventStream;
  extern int _nLinuxInput_EventStream_CurSlot;

  // Library main access
  int LinuxInput_InitTouch();
  bool LinuxInput_UpdateTouch();
  int LinuxInput_CloseTouch();

  // Library swipe controll
  void LinuxInput_ApplyRestart();
  void LinuxInput_ApplyShutdown();
  void LinuxInput_UpdateSwipe();

  #ifdef __cplusplus
  }
  #endif

#endif // _LIBTOUCH_H_
