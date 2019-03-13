#ifndef PUTNO_CONFIG
#   define PUTNO_CONFIG


// DEFAULT_HOTKEY_NUMBER macro must be at least SET. Values supported:
//     2 - for Ctrl+Shift hotkey
//     4 - for [`~] hotkey
//     1 (or any other) - Alt+Shift (default)
#   define DEFAULT_HOTKEY_NUMBER 1

// Detection requires Registry access which significally affects the app's size :(
// But it is handful! So you may chose (0 = disabled, DEFAULT_HOTKEY_NUMBER macro will used;
// 1 = enabled, compiler will produce larger file):
#   define AUTODETECT_HOTKEY 1

// Single word buffer size:
    const int BUFFER_SIZE=500;


///////////////////////////////////////////////////////////////
// CALCULATIONS (changes not recommended):
//

#   if   DEFAULT_HOTKEY_NUMBER==2
        DWORD key1 = 162, key2 = 160;
#   elif DEFAULT_HOTKEY_NUMBER==4
#       if AUTODETECT_HOTKEY==1
            DWORD key1 = 192, key2 = 0;
#       else
            DWORD key1 = 192;
#       endif
#   else
        DWORD key1 = 164, key2=160;
#   endif

#endif //ifndef PUTNO_CONFIG
