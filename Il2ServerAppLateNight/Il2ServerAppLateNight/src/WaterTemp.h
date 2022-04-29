#pragma once
#include <windows.h>
#include <vector>

std::vector<double> ReadWaterTemps(LPCVOID codeCaveAddress, HANDLE hProcess);

bool HookWaterTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);
