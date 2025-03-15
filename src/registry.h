#pragma once
#include <windows.h>
#include <string>

bool readSettingsFromRegistry();
void writeSettingsToRegistry();
void RegisterKeyboardInput(HWND hwnd);
std::string getKeyName(int vkCode);