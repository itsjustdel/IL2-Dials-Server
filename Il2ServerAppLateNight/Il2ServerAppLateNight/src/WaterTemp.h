#pragma once
#include <windows.h>
#include <vector>

std::vector<double> ReadWaterTemps(HANDLE hProcess, LPVOID codeCaveAddress );

bool HookWaterTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);
