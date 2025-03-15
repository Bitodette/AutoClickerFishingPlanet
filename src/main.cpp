#define UNICODE
#define _UNICODE
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include <conio.h>
#include <map>
#include <iomanip>
#include <hidusage.h>
#include "auto_clicker.h"
#include "registry.h"

// Global variables (initialized here, declared extern in headers or other .cpp)
HINSTANCE hInst;
bool isAutoClickerRunning = false;
bool isAutoClickerPaused = false;
bool isCatchClicking = false;

int basePullDuration = 1950;
int baseStopDuration = 850;
int margin = 10;
int stopGoKey = 0;
int catchKey = 0;

bool isShortcutsEnabled = false;
bool isSettingKey = false;

// UI handles
HWND hwndStartStop, hwndPullDuration, hwndStopDuration;
HWND hwndStopGoKey, hwndCatchKey, hwndStatus;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;
    srand((unsigned)time(nullptr));

    // Load settings if available
    bool settingsLoaded = readSettingsFromRegistry();

    const char CLASS_NAME[] = "AutoClickerWindowClass";
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Auto Clicker V2.0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwnd)
    {
        MessageBoxA(NULL, "Window creation failed!", "Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
        // Example: if a "catch hold" feature is needed
        break;

    case WM_CREATE:
    {
        // Create UI elements
        hwndStartStop = CreateWindowA(
            "BUTTON", "Start",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            50, 20, 100, 30,
            hwnd, (HMENU)1, hInst, NULL);

        CreateWindowA("STATIC", "Pull Duration (ms):",
                      WS_VISIBLE | WS_CHILD,
                      20, 70, 150, 20,
                      hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "Stop Duration (ms):",
                      WS_VISIBLE | WS_CHILD,
                      20, 110, 150, 20,
                      hwnd, NULL, NULL, NULL);

        char pullBuf[16] = {0};
        char stopBuf[16] = {0};

        // Use _snprintf_s for safe string formatting
        _snprintf_s(pullBuf, sizeof(pullBuf), _TRUNCATE, "%d", basePullDuration);
        _snprintf_s(stopBuf, sizeof(stopBuf), _TRUNCATE, "%d", baseStopDuration);

        hwndPullDuration = CreateWindowA("EDIT", pullBuf,
                                         WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
                                         180, 70, 100, 20,
                                         hwnd, (HMENU)2, hInst, NULL);

        hwndStopDuration = CreateWindowA("EDIT", stopBuf,
                                         WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
                                         180, 110, 100, 20,
                                         hwnd, (HMENU)3, hInst, NULL);

        CreateWindowA("STATIC", "Stop & Go Key:",
                      WS_VISIBLE | WS_CHILD,
                      20, 150, 150, 20,
                      hwnd, NULL, NULL, NULL);

        hwndStopGoKey = CreateWindowA(
            "BUTTON",
            (stopGoKey != 0) ? getKeyName(stopGoKey).c_str() : "Unknown",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            180, 150, 100, 25,
            hwnd, (HMENU)4, hInst, NULL);

        CreateWindowA("STATIC", "Catch Key:",
                      WS_VISIBLE | WS_CHILD,
                      20, 190, 150, 20,
                      hwnd, NULL, NULL, NULL);

        hwndCatchKey = CreateWindowA(
            "BUTTON",
            (catchKey != 0) ? getKeyName(catchKey).c_str() : "Unknown",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            180, 190, 100, 25,
            hwnd, (HMENU)5, hInst, NULL);

        CreateWindowA(
            "BUTTON", "Save",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            180, 230, 100, 25,
            hwnd, (HMENU)6, hInst, NULL);

        hwndStatus = CreateWindowA("STATIC", "Inactive",
                                   WS_VISIBLE | WS_CHILD,
                                   180, 20, 100, 30,
                                   hwnd, NULL, NULL, NULL);

        SetTimer(hwnd, 1, 50, NULL);
        RegisterKeyboardInput(hwnd);
    }
    break;

    case WM_INPUT:
    {
        if (isSettingKey || !isShortcutsEnabled)
            break;

        UINT dwSize = sizeof(RAWINPUT);
        BYTE lpb[sizeof(RAWINPUT)];
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) <= 0)
            break;

        RAWINPUT *raw = (RAWINPUT *)lpb;
        if (stopGoKey == 0 || catchKey == 0)
            break;

        if (raw->header.dwType == RIM_TYPEKEYBOARD)
        {
            USHORT vKey = raw->data.keyboard.VKey;
            USHORT flags = raw->data.keyboard.Flags;
            if (flags == RI_KEY_MAKE)
            {
                // Stop & Go
                if (vKey == stopGoKey)
                {
                    if (isCatchClicking)
                    {
                        isCatchClicking = false;
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    isAutoClickerRunning = !isAutoClickerRunning;
                    SetWindowTextA(hwndStatus, isAutoClickerRunning ? "Stop & Go Active" : "Inactive");

                    if (isAutoClickerRunning)
                    {
                        std::thread(autoClickerThread).detach();
                    }
                    else
                    {
                        INPUT input = {0};
                        input.type = INPUT_MOUSE;
                        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                        SendInput(1, &input, sizeof(INPUT));
                    }
                }
                // Catch
                else if (vKey == catchKey)
                {
                    if (isAutoClickerRunning)
                    {
                        isAutoClickerRunning = false;
                        INPUT input = {0};
                        input.type = INPUT_MOUSE;
                        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                        SendInput(1, &input, sizeof(INPUT));
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    isCatchClicking = !isCatchClicking;
                    if (isCatchClicking)
                    {
                        std::thread(catchClickerThread).detach();
                        SetWindowTextA(hwndStatus, "Catch Active");
                    }
                    else
                    {
                        SetWindowTextA(hwndStatus, "Inactive");
                    }
                }
            }
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            USHORT btnFlags = raw->data.mouse.usButtonFlags;
            bool xbtn1Down = (btnFlags & RI_MOUSE_BUTTON_4_DOWN) != 0;
            bool xbtn2Down = (btnFlags & RI_MOUSE_BUTTON_5_DOWN) != 0;

            if (xbtn1Down && stopGoKey == VK_XBUTTON1)
            {
                if (isCatchClicking)
                {
                    isCatchClicking = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                isAutoClickerRunning = !isAutoClickerRunning;
                SetWindowTextA(hwndStatus, isAutoClickerRunning ? "Stop & Go Active" : "Inactive");

                if (isAutoClickerRunning)
                {
                    std::thread(autoClickerThread).detach();
                }
                else
                {
                    INPUT input = {0};
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                }
            }
            else if (xbtn2Down && stopGoKey == VK_XBUTTON2)
            {
                if (isCatchClicking)
                {
                    isCatchClicking = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                isAutoClickerRunning = !isAutoClickerRunning;
                SetWindowTextA(hwndStatus, isAutoClickerRunning ? "Stop & Go Active" : "Inactive");

                if (isAutoClickerRunning)
                {
                    std::thread(autoClickerThread).detach();
                }
                else
                {
                    INPUT input = {0};
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                }
            }
            else if (xbtn1Down && catchKey == VK_XBUTTON1)
            {
                if (isAutoClickerRunning)
                {
                    isAutoClickerRunning = false;
                    INPUT input = {0};
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                isCatchClicking = !isCatchClicking;
                if (isCatchClicking)
                {
                    std::thread(catchClickerThread).detach();
                    SetWindowTextA(hwndStatus, "Catch Active");
                }
                else
                {
                    SetWindowTextA(hwndStatus, "Inactive");
                }
            }
            else if (xbtn2Down && catchKey == VK_XBUTTON2)
            {
                if (isAutoClickerRunning)
                {
                    isAutoClickerRunning = false;
                    INPUT input = {0};
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                isCatchClicking = !isCatchClicking;
                if (isCatchClicking)
                {
                    std::thread(catchClickerThread).detach();
                    SetWindowTextA(hwndStatus, "Catch Active");
                }
                else
                {
                    SetWindowTextA(hwndStatus, "Inactive");
                }
            }
        }
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        // Start/Stop
        case 1:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (!isShortcutsEnabled)
                {
                    char pullBuf[16] = {0}, stopBuf[16] = {0};
                    GetWindowTextA(hwndPullDuration, pullBuf, sizeof(pullBuf));
                    GetWindowTextA(hwndStopDuration, stopBuf, sizeof(stopBuf));

                    basePullDuration = atoi(pullBuf);
                    baseStopDuration = atoi(stopBuf);
                    isShortcutsEnabled = true;

                    SetWindowTextA(hwndStartStop, "Stop");
                }
                else
                {
                    isShortcutsEnabled = false;
                    isAutoClickerRunning = false;
                    isCatchClicking = false;

                    SetWindowTextA(hwndStartStop, "Start");
                    SetWindowTextA(hwndStatus, "Inactive");

                    // Release mouse if held
                    INPUT input = {0};
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                }
            }
            break;

        // Save
        case 6:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                char pullBuf[16] = {0}, stopBuf[16] = {0};
                GetWindowTextA(hwndPullDuration, pullBuf, sizeof(pullBuf));
                GetWindowTextA(hwndStopDuration, stopBuf, sizeof(stopBuf));

                basePullDuration = atoi(pullBuf);
                baseStopDuration = atoi(stopBuf);

                writeSettingsToRegistry();
                MessageBoxA(hwnd, "Settings saved successfully!", "Success", MB_OK | MB_ICONINFORMATION);
            }
            break;

        // Set Stop & Go Key
        case 4:
        {
            isSettingKey = true;
            MessageBoxA(hwnd, "Press any key or mouse XButton for Stop & Go", "Set Key", MB_OK | MB_ICONINFORMATION);
            bool keySet = false;

            while (!keySet)
            {
                Sleep(10);

                // Check mouse side buttons
                if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
                {
                    if (catchKey == VK_XBUTTON1)
                    {
                        MessageBoxA(hwnd, "XButton1 is already used for Catch!", "Warning", MB_OK | MB_ICONWARNING);
                        continue;
                    }
                    stopGoKey = VK_XBUTTON1;
                    std::string name = getKeyName(stopGoKey);
                    if (name != "Unknown")
                    {
                        SetWindowTextA(hwndStopGoKey, name.c_str());
                        MessageBoxA(hwnd, ("Stop & Go key set to: " + name).c_str(), "Key Set", MB_OK | MB_ICONINFORMATION);
                        keySet = true;
                    }
                }
                else if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
                {
                    if (catchKey == VK_XBUTTON2)
                    {
                        MessageBoxA(hwnd, "XButton2 is already used for Catch!", "Warning", MB_OK | MB_ICONWARNING);
                        continue;
                    }
                    stopGoKey = VK_XBUTTON2;
                    std::string name = getKeyName(stopGoKey);
                    if (name != "Unknown")
                    {
                        SetWindowTextA(hwndStopGoKey, name.c_str());
                        MessageBoxA(hwnd, ("Stop & Go key set to: " + name).c_str(), "Key Set", MB_OK | MB_ICONINFORMATION);
                        keySet = true;
                    }
                }
                else
                {
                    // Check keyboard keys
                    for (int vk = 1; vk < 256; vk++)
                    {
                        if (GetAsyncKeyState(vk) & 0x8000)
                        {
                            if (vk == catchKey)
                            {
                                MessageBoxA(hwnd, "That key is already used for Catch!", "Warning", MB_OK | MB_ICONWARNING);
                                continue;
                            }
                            stopGoKey = vk;
                            std::string name = getKeyName(stopGoKey);
                            if (name != "Unknown")
                            {
                                SetWindowTextA(hwndStopGoKey, name.c_str());
                                MessageBoxA(hwnd, ("Stop & Go key set to: " + name).c_str(), "Key Set", MB_OK | MB_ICONINFORMATION);
                                keySet = true;
                                break;
                            }
                        }
                    }
                }
            }

            isSettingKey = false;
            writeSettingsToRegistry();
        }
        break;

        // Set Catch Key
        case 5:
        {
            isSettingKey = true;
            MessageBoxA(hwnd, "Press any key or mouse XButton for Catch", "Set Key", MB_OK | MB_ICONINFORMATION);
            bool keySet = false;

            while (!keySet)
            {
                Sleep(10);

                // Check mouse side buttons
                if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
                {
                    if (stopGoKey == VK_XBUTTON1)
                    {
                        MessageBoxA(hwnd, "XButton1 is already used for Stop & Go!", "Warning", MB_OK | MB_ICONWARNING);
                        continue;
                    }
                    catchKey = VK_XBUTTON1;
                    std::string name = getKeyName(catchKey);
                    if (name != "Unknown")
                    {
                        SetWindowTextA(hwndCatchKey, name.c_str());
                        MessageBoxA(hwnd, ("Catch key set to: " + name).c_str(), "Key Set", MB_OK | MB_ICONINFORMATION);
                        keySet = true;
                    }
                }
                else if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
                {
                    if (stopGoKey == VK_XBUTTON2)
                    {
                        MessageBoxA(hwnd, "XButton2 is already used for Stop & Go!", "Warning", MB_OK | MB_ICONWARNING);
                        continue;
                    }
                    catchKey = VK_XBUTTON2;
                    std::string name = getKeyName(catchKey);
                    if (name != "Unknown")
                    {
                        SetWindowTextA(hwndCatchKey, name.c_str());
                        MessageBoxA(hwnd, ("Catch key set to: " + name).c_str(), "Key Set", MB_OK | MB_ICONINFORMATION);
                        keySet = true;
                    }
                }
                else
                {
                    // Check keyboard keys
                    for (int vk = 1; vk < 256; vk++)
                    {
                        if (GetAsyncKeyState(vk) & 0x8000)
                        {
                            if (vk == stopGoKey)
                            {
                                MessageBoxA(hwnd, "That key is already used for Stop & Go!", "Warning", MB_OK | MB_ICONWARNING);
                                continue;
                            }
                            catchKey = vk;
                            std::string name = getKeyName(catchKey);
                            if (name != "Unknown")
                            {
                                SetWindowTextA(hwndCatchKey, name.c_str());
                                MessageBoxA(hwnd, ("Catch key set to: " + name).c_str(), "Key Set", MB_OK | MB_ICONINFORMATION);
                                keySet = true;
                                break;
                            }
                        }
                    }
                }
            }

            isSettingKey = false;
            writeSettingsToRegistry();
        }
        break;

        default:
            break;
        }
    }
    break;

    case WM_TIMER:
        if (isSettingKey)
            break;
        break;

    case WM_DESTROY:
        writeSettingsToRegistry();
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}