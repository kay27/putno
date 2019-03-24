// Putno Fixer by kay27
// version 1.6 don't compile - it will break your pc
// github.com/kay27/putno

#define _WIN32_WINNT 0x0400
#pragma comment(lib, "user32.lib")

#include <windows.h>
#include <vector>
#include <string>

#include "config.h"

HHOOK hKeyboardHook;

KBDLLHOOKSTRUCT buffer[BUFFER_SIZE]; // .flags now used to store Shift state

int counter = 0, fixing = 0;
SHORT shift = 0, control = 0, alt = 0;

#if BUFFER_TIMEOUT_SECONDS > 0
    DWORD lastTime = 0;
#endif

#if AUTODETECT_HOTKEY==1
    char hotkey;
#endif

#if   DEFAULT_HOTKEY_NUMBER==2
    DWORD key1 = 162, key2 = 160;
#elif DEFAULT_HOTKEY_NUMBER==4
#   if AUTODETECT_HOTKEY==1
        DWORD key1 = 192, key2 = 0;
#   else
        DWORD key1 = 192;
#   endif
#else
    DWORD key1 = 164, key2=160;
#endif
