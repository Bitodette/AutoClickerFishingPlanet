#pragma once
#include <windows.h>

// External variables shared across files
extern bool isAutoClickerRunning;
extern bool isAutoClickerPaused;
extern bool isCatchClicking;

// Function declarations
void autoClicker(int basePullDuration, int baseStopDuration, int margin);
void catchClicker();
void autoClickerPaused();
void autoClickerThread();
void autoClickerPausedThread();
void catchClickerThread();