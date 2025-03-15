#include "registry.h"
#include <map>
#include <ctime>
#include <hidusage.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <tchar.h>
#include <iostream>

// External references (defined elsewhere)
extern int basePullDuration;
extern int baseStopDuration;
extern int stopGoKey;
extern int catchKey;

// Read settings from the registry
bool readSettingsFromRegistry()
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\MyClicker"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD dataSize = sizeof(DWORD);
        DWORD val1, val2, val3, val4;
        RegQueryValueEx(hKey, TEXT("PullDuration"), NULL, NULL, (LPBYTE)&val1, &dataSize);
        RegQueryValueEx(hKey, TEXT("StopDuration"), NULL, NULL, (LPBYTE)&val2, &dataSize);
        RegQueryValueEx(hKey, TEXT("StopGoKey"), NULL, NULL, (LPBYTE)&val3, &dataSize);
        RegQueryValueEx(hKey, TEXT("CatchKey"), NULL, NULL, (LPBYTE)&val4, &dataSize);

        basePullDuration = val1;
        baseStopDuration = val2;
        stopGoKey = val3;
        catchKey = val4;

        RegCloseKey(hKey);
        return true;
    }
    return false;
}

// Write settings to the registry
void writeSettingsToRegistry()
{
    HKEY hKey;
    DWORD disp;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\MyClicker"), 0, NULL,
                       REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &disp) == ERROR_SUCCESS)
    {
        DWORD val1 = basePullDuration;
        DWORD val2 = baseStopDuration;
        DWORD val3 = stopGoKey;
        DWORD val4 = catchKey;

        RegSetValueEx(hKey, TEXT("PullDuration"), 0, REG_DWORD, (BYTE *)&val1, sizeof(val1));
        RegSetValueEx(hKey, TEXT("StopDuration"), 0, REG_DWORD, (BYTE *)&val2, sizeof(val2));
        RegSetValueEx(hKey, TEXT("StopGoKey"), 0, REG_DWORD, (BYTE *)&val3, sizeof(val3));
        RegSetValueEx(hKey, TEXT("CatchKey"), 0, REG_DWORD, (BYTE *)&val4, sizeof(val4));
        RegCloseKey(hKey);
    }
}

// Register for raw keyboard input
void RegisterKeyboardInput(HWND hwnd)
{
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE)))
    {
        MessageBoxA(NULL, "Failed to register raw input device", "Error", MB_ICONERROR);
    }
}

// Convert VK code to a descriptive string
std::string getKeyName(int vkCode)
{
    if (vkCode == 0)
        return "Unknown";

    static std::map<int, std::string> keyMap = {
        {VK_F1, "F1"}, {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"}, {VK_F5, "F5"},
        {VK_F6, "F6"}, {VK_F7, "F7"}, {VK_F8, "F8"}, {VK_F9, "F9"}, {VK_F10, "F10"},
        {VK_F11, "F11"}, {VK_F12, "F12"}, {VK_UP, "Up Arrow"}, {VK_DOWN, "Down Arrow"},
        {VK_LEFT, "Left Arrow"}, {VK_RIGHT, "Right Arrow"}, {VK_PRIOR, "Page Up"},
        {VK_NEXT, "Page Down"}, {VK_HOME, "Home"}, {VK_END, "End"}, {VK_INSERT, "Insert"},
        {VK_DELETE, "Delete"}, {VK_ESCAPE, "Escape"}, {VK_TAB, "Tab"}, {VK_CAPITAL, "Caps Lock"},
        {VK_SHIFT, "Shift"}, {VK_CONTROL, "Control"}, {VK_MENU, "Alt"}, {VK_SPACE, "Space"},
        {VK_RETURN, "Enter"}, {VK_BACK, "Backspace"}, {VK_NUMLOCK, "Num Lock"},
        {VK_SCROLL, "Scroll Lock"}, {VK_OEM_1, ";"}, {VK_OEM_PLUS, "="}, {VK_OEM_COMMA, ","},
        {VK_OEM_MINUS, "-"}, {VK_OEM_PERIOD, "."}, {VK_OEM_2, "/"}, {VK_OEM_3, "`"},
        {VK_OEM_4, "["}, {VK_OEM_5, "\\"}, {VK_OEM_6, "]"}, {VK_OEM_7, "'"}, {VK_OEM_8, "OEM_8"},
        {VK_NUMPAD0, "Numpad 0"}, {VK_NUMPAD1, "Numpad 1"}, {VK_NUMPAD2, "Numpad 2"},
        {VK_NUMPAD3, "Numpad 3"}, {VK_NUMPAD4, "Numpad 4"}, {VK_NUMPAD5, "Numpad 5"},
        {VK_NUMPAD6, "Numpad 6"}, {VK_NUMPAD7, "Numpad 7"}, {VK_NUMPAD8, "Numpad 8"},
        {VK_NUMPAD9, "Numpad 9"}, {VK_MULTIPLY, "Numpad *"}, {VK_ADD, "Numpad +"},
        {VK_SUBTRACT, "Numpad -"}, {VK_DECIMAL, "Numpad ."}, {VK_DIVIDE, "Numpad /"},
        {VK_VOLUME_MUTE, "Volume Mute"}, {VK_VOLUME_DOWN, "Volume Down"}, {VK_VOLUME_UP, "Volume Up"},
        {VK_MEDIA_NEXT_TRACK, "Next Track"}, {VK_MEDIA_PREV_TRACK, "Previous Track"},
        {VK_MEDIA_STOP, "Stop Media"}, {VK_MEDIA_PLAY_PAUSE, "Play/Pause"},
        {VK_BROWSER_BACK, "Browser Back"}, {VK_BROWSER_FORWARD, "Browser Forward"},
        {VK_BROWSER_REFRESH, "Browser Refresh"}, {VK_BROWSER_STOP, "Browser Stop"},
        {VK_BROWSER_SEARCH, "Browser Search"}, {VK_BROWSER_FAVORITES, "Browser Favorites"},
        {VK_BROWSER_HOME, "Browser Home"}, {VK_SLEEP, "Sleep"}, {VK_APPS, "Menu"},
        {VK_SNAPSHOT, "Print Screen"}, {VK_PAUSE, "Pause"}
    };

    // If recognized in the map
    if (keyMap.find(vkCode) != keyMap.end())
        return keyMap[vkCode];

    // Check for alpha or digit
    if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9'))
        return std::string(1, static_cast<char>(vkCode));

    if (vkCode == VK_XBUTTON1) return "XButton1";
    if (vkCode == VK_XBUTTON2) return "XButton2";

    // Otherwise try system name
    char keyName[256];
    UINT scanCode = MapVirtualKeyA(vkCode, MAPVK_VK_TO_VSC);
    bool isExtended = false;
    switch (vkCode)
    {
    case VK_RIGHT: case VK_LEFT:  case VK_UP:    case VK_DOWN:
    case VK_PRIOR: case VK_NEXT:  case VK_END:   case VK_HOME:
    case VK_INSERT: case VK_DELETE: case VK_DIVIDE: case VK_NUMLOCK:
        isExtended = true;
        break;
    default:
        isExtended = false;
    }

    LONG lparam = (scanCode << 16);
    if (isExtended)
        lparam |= 0x01000000;

    if (GetKeyNameTextA(lparam, keyName, sizeof(keyName)) > 0)
    {
        std::string name(keyName);
        if (!name.empty())
            name[0] = toupper(name[0]);
        return name;
    }

    return "Unknown";
}