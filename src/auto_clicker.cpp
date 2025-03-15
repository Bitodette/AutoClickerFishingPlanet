#include "auto_clicker.h"
#include <thread>
#include <chrono>

using namespace std;

// Implementation for auto-clicker logic
void autoClicker(int basePullDuration, int baseStopDuration, int margin)
{
    while (isAutoClickerRunning)
    {
        // Press left mouse
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));

        // If user stopped mid-hold, release and break
        if (!isAutoClickerRunning)
        {
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(INPUT));
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(basePullDuration));

        // Release left mouse
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));

        // If user stopped mid-stop period, break
        if (!isAutoClickerRunning)
            break;

        this_thread::sleep_for(chrono::milliseconds(baseStopDuration));
    }
}

// Implementation for paused auto-clicker
void autoClickerPaused()
{
    while (isAutoClickerPaused)
    {
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
        this_thread::sleep_for(chrono::milliseconds(50));

        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

// Implementation for “catch” clicker
void catchClicker()
{
    while (isCatchClicking)
    {
        INPUT input = {0};
        input.type = INPUT_MOUSE;

        // Press left mouse
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
        this_thread::sleep_for(chrono::milliseconds(50));

        // Release left mouse
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

// Threads
void autoClickerThread()
{
    // Example margin passing (unused here)
    autoClicker(1950, 850, 10);
}

void autoClickerPausedThread()
{
    autoClickerPaused();
}

void catchClickerThread()
{
    catchClicker();
}