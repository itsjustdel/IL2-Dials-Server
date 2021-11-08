#pragma once
#include <Windows.h>
#include <string>

uintptr_t OffsetToTurnNeedle(std::string planeType);

LPCVOID TurnNeedleScanner(LPCVOID structStart, HANDLE hProcess, bool injectedTurnNeedle, LPVOID codeCaveAddress, HANDLE hProcessIL2);