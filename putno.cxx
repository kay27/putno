#include "putno.h"

void Switch()
{
    INPUT inp;
    inp.type=INPUT_KEYBOARD;
    inp.ki.wScan = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;
    inp.ki.dwFlags = 0;
    inp.ki.wVk = key1;
    SendInput(1, &inp, sizeof(INPUT));
#   if AUTODETECT_HOTKEY == 1 || DEFAULT_HOTKEY_NUMBER != 4
#       if AUTODETECT_HOTKEY == 1
            if(key2)
            {
#       endif
                inp.ki.wVk = key2;
                SendInput(1, &inp, sizeof(INPUT));
                inp.ki.dwFlags = KEYEVENTF_KEYUP; 
                SendInput(1, &inp, sizeof(INPUT));
                inp.ki.wVk = key1;
#       if AUTODETECT_HOTKEY == 1
            }
            else
#       endif
#   endif
            inp.ki.dwFlags = KEYEVENTF_KEYUP; 

    SendInput(1, &inp, sizeof(INPUT));
}


inline bool FixLastWord()
{
    if(counter==0) return false;

    fixing = 1;

    INPUT inp;
    inp.type=INPUT_KEYBOARD;
    inp.ki.wScan = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;
    inp.ki.wVk = 8;
    for(auto i=0; i<counter; i++)
    {
      inp.ki.dwFlags = 0;
      SendInput(1, &inp, sizeof(INPUT));
      inp.ki.dwFlags = KEYEVENTF_KEYUP; 
      SendInput(1, &inp, sizeof(INPUT));
    }

    Switch();

    for(auto i=0; i<counter; i++)
    {
      SHORT shift0 = (SHORT)buffer[i].flags;
      inp.ki.dwFlags = 0;
      if(shift0)
      {
          inp.ki.wVk = VK_SHIFT;
          inp.ki.wScan = 0;
          SendInput(1, &inp, sizeof(INPUT));
      }
      inp.ki.wVk = buffer[i].vkCode;
      inp.ki.wScan = buffer[i].scanCode;
      SendInput(1, &inp, sizeof(INPUT));
      inp.ki.dwFlags = KEYEVENTF_KEYUP; 
      SendInput(1, &inp, sizeof(INPUT));
      if(shift0)
      {
          inp.ki.wVk = VK_SHIFT;
          inp.ki.wScan = 0;
          SendInput(1, &inp, sizeof(INPUT));
      }
    }

    fixing = 0;

    return true;
}


inline bool FixText(const wchar_t* text)
{
    auto len = std::wcslen(text);
    int j=0;
    SHORT buffer[len];
    LANGID oldLang = GetUserDefaultUILanguage();
    LANGID lang = oldLang;
    LANGID lang0 = oldLang;
    for(auto i=0; i<len; i++)
    {
        WCHAR w = text[i];
        SHORT code;
        do
        {
            code = VkKeyScanW(w);
            if(code < 0)
            {
                lang0 = lang;
                Switch();
                lang = GetUserDefaultUILanguage();
            }
        } while((code<0)&&(lang!=lang0));

        if(code < 0)
            continue;
        buffer[j++] = code;
    }
    len = j;

    for(auto i=1; i<5; i++)
    {
        if(GetUserDefaultUILanguage() == oldLang)
            break;
        Switch();
    }

    Switch();

    INPUT inp;
    inp.type=INPUT_KEYBOARD;
    inp.ki.wScan = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;

    for(auto i=0; i<len; i++)
    {
        auto vk = buffer[i] & 0xFF;
        if( ((vk >= 48)&&(vk <= 90)) || ((vk >= 96)&&(vk <= 111)) || ((vk >= 186)&&(vk <= 222)) || (vk==9) || (vk==13) || (vk==32)) 
        {
            bool shift = buffer[i] & 0x100;

            inp.ki.dwFlags = 0;
            if(shift)
            {
                inp.ki.wVk = VK_SHIFT;
                SendInput(1, &inp, sizeof(INPUT));
            }

            inp.ki.wVk = vk;
            SendInput(1, &inp, sizeof(INPUT));

            inp.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &inp, sizeof(INPUT));

            if(shift)
            {
                inp.ki.wVk = VK_SHIFT;
                SendInput(1, &inp, sizeof(INPUT));
            }
        }
    }

    for(auto i=1; i<100; i++)
    {
        if(GetUserDefaultUILanguage() == oldLang)
            break;
        Switch();
    }

}


inline bool FixSelection()
{
    fixing = 1;

    std::vector<std::pair<UINT, HGLOBAL>> cb_backup;
//    if((CountClipboardFormats() > 0) && OpenClipboard(NULL))
//    {
//        UINT cur_fmt = 0;
//        while((cur_fmt = EnumClipboardFormats(cur_fmt)) != 0)
//            cb_backup.push_back(std::make_pair(cur_fmt, GetClipboardData(cur_fmt)));
//        CloseClipboard();
//    }

    // Unpress Shift key:

    INPUT inp;
    inp.type=INPUT_KEYBOARD;
//    inp.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
    inp.ki.wScan = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;
    inp.ki.wVk = VK_SHIFT;
    inp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inp, sizeof(INPUT));

    // Press Ctrl+C (and unpress):

    inp.ki.wVk = VK_CONTROL;
    inp.ki.dwFlags = 0;
    SendInput(1, &inp, sizeof(INPUT));
    inp.ki.wVk = 0x56;
    SendInput(1, &inp, sizeof(INPUT));
    inp.ki.dwFlags = KEYEVENTF_KEYUP;;
    SendInput(1, &inp, sizeof(INPUT));
    inp.ki.wVk = VK_CONTROL;
    SendInput(1, &inp, sizeof(INPUT));

    Sleep(1);

    wchar_t* text = nullptr;
    if(OpenClipboard(NULL))
    {
        HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
        if(hglb != NULL)
        {
            wchar_t* str = (wchar_t*)GlobalLock(hglb);
            if(str)
            {
                auto byteLength = GlobalSize(str);
                text = (wchar_t*)memcpy(new wchar_t[byteLength], str, byteLength);
                GlobalUnlock(hglb);
            }
        }
        CloseClipboard();
    }

//    if(!text.IsEmpty())
        FixText(text);

//    cbbackup.Restore();

    // Press Shift key back:

    inp.ki.wVk = VK_SHIFT;
    inp.ki.dwFlags = 0;
    SendInput(1, &inp, sizeof(INPUT));

    fixing = 0;

    return true;
}


__declspec(dllexport) LRESULT CALLBACK KeyboardEvent (int nCode, WPARAM wParam, LPARAM lParam)
{
    if((nCode != HC_ACTION) || ((wParam != WM_KEYDOWN)&&(wParam != WM_KEYUP)))
        return CallNextHookEx(hKeyboardHook, nCode,wParam,lParam);

    KBDLLHOOKSTRUCT hook = *((KBDLLHOOKSTRUCT*)lParam);
    DWORD vkCode = hook.vkCode;

    if((vkCode == VK_SHIFT  )||(vkCode == VK_LSHIFT)  ||(vkCode == VK_RSHIFT  )) shift   = (wParam == WM_KEYDOWN);
    if((vkCode == VK_CONTROL)||(vkCode == VK_LCONTROL)||(vkCode == VK_RCONTROL)) control = (wParam == WM_KEYDOWN);
    if((vkCode == VK_MENU   )||(vkCode == VK_LMENU   )||(vkCode == VK_RMENU   )) alt     = (wParam == WM_KEYDOWN);

    if(fixing) return 0;

    if((vkCode == 0)||(wParam != WM_KEYDOWN))
        return CallNextHookEx(hKeyboardHook, nCode,wParam,lParam);

#if BUFFER_TIMEOUT_SECONDS > 0
    if(hook.time - lastTime >= 1000 * BUFFER_TIMEOUT_SECONDS) // milliseconds
        counter = 0;
    lastTime = hook.time;
#endif

    if ((!(shift||control||alt)) && (vkCode == VK_PAUSE))
    {
        if(FixLastWord()) return 0;
    }

    else if (shift && (!(control||alt)) && (vkCode == VK_PAUSE))
    {
        if(FixSelection()) return 0;
    }

    else if((vkCode == 8)||(vkCode == 37)) // backspace, arrow left
    {
        if(counter>0) counter--;
    }

    else if(    ( ((vkCode >= 48)&&(vkCode <= 90)) || ((vkCode >= 96)&&(vkCode <= 111)) || ((vkCode >= 186)&&(vkCode <= 222)) )
             && (!(control||alt))    )
    {
        buffer[counter]       = hook;
        buffer[counter].flags = shift;
        if((++counter) >= BUFFER_SIZE)
        {
            counter = BUFFER_SIZE/2;
            memcpy(buffer, &buffer[BUFFER_SIZE/2], BUFFER_SIZE / 2 * sizeof(KBDLLHOOKSTRUCT));
        }
    }

    else
    {
        if((vkCode&(~1)) != 160) //both shift keys
            counter = 0;
    }

    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

#if AUTODETECT_HOTKEY==1
    LPBYTE ReadRegValue(HKEY root, LPCSTR key, LPCSTR lpValueName, long unsigned int & lpcbData /* max length (bytes) */)
    {
        HKEY hKey;
        if (RegOpenKeyExA(root, key, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            LPBYTE lpData = new BYTE[lpcbData];
            if(RegQueryValueExA(hKey, lpValueName, NULL, NULL, lpData, &lpcbData) == ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
                return lpData;
            }
            RegCloseKey(hKey);
        }
        return nullptr;
    }
#endif


int main(int argc, char** argv)
{
    CreateMutex(NULL, TRUE, "PutnoFixer");
    if(GetLastError() == ERROR_ALREADY_EXISTS)
        return 1;

#   if AUTODETECT_HOTKEY==1
        hotkey = '1'; // Alt+Shift is default
        {
            long unsigned int tempSize = 4;
            LPBYTE tempBuf = ReadRegValue(HKEY_CURRENT_USER, "Keyboard Layout\\Toggle", "Hotkey", tempSize);
            if(tempBuf != nullptr) { hotkey = tempBuf[0]; delete tempBuf; }
        }
        if(hotkey == '2') // Control+Shift
            key1 = 162;
        else if(hotkey == '4') // [~`] key
        {
            key1 = 192; key2 = 0;
        }
#   endif

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEvent, NULL, 0);
    MSG message;
    while (GetMessage(&message,NULL,0,0) > 0) 
    {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}
