// Putno Fixer by kay27
// version 1.2 for mingw-w64
// mailto: kay27@bk.ru

#define _WIN32_WINNT 0x0400
#pragma comment(lib, "user32.lib")

#include <windows.h>
#include <vector>
#include <string>

const int BUFFER_SIZE=500;
HHOOK hKeyboardHook;
KBDLLHOOKSTRUCT buffer[BUFFER_SIZE];
int counter = 0, fixing = 0;
SHORT shift = 0, control = 0, alt = 0;
char hotkey;
DWORD key1=164, key2=160;

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

    inp.ki.dwFlags = 0;
    inp.ki.wVk = key1;
    SendInput(1, &inp, sizeof(INPUT));

    if(key2)
    {
        inp.ki.wVk = key2;
        SendInput(1, &inp, sizeof(INPUT));
        inp.ki.dwFlags = KEYEVENTF_KEYUP; 
        SendInput(1, &inp, sizeof(INPUT));
        inp.ki.wVk = key1;
    }
    else
        inp.ki.dwFlags = KEYEVENTF_KEYUP; 

    SendInput(1, &inp, sizeof(INPUT));

    for(auto i=0; i<counter; i++)
    {
      inp.ki.wVk = buffer[i].vkCode;
      inp.ki.wScan = buffer[i].scanCode;
      inp.ki.dwFlags = 0;
      SendInput(1, &inp, sizeof(INPUT));
      inp.ki.dwFlags = KEYEVENTF_KEYUP; 
      SendInput(1, &inp, sizeof(INPUT));
    }

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

    if ((!(shift||control||alt)) && (vkCode == VK_PAUSE))
    {
        if(FixLastWord()) return 0;
    }

    else if((vkCode == 8)||(vkCode == 37)) // backspace, arrow left
    {
        if(counter>0) counter--;
    }

    else if( ((vkCode >= 48)&&(vkCode <= 90)) || ((vkCode >= 96)&&(vkCode <= 111)) || ((vkCode >= 186)&&(vkCode <= 222)))
    {
        buffer[counter++]=hook;
        if(counter >= BUFFER_SIZE)
        {
            counter = BUFFER_SIZE/2;
            memcpy(buffer, &buffer[BUFFER_SIZE/2], BUFFER_SIZE / 2 * sizeof(KBDLLHOOKSTRUCT));
        }
    }

    else
    {
        counter = 0;
    }

    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

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

int main(int argc, char** argv)
{
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
