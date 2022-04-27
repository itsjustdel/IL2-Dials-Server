#pragma once
#include <vector>
#include <wtypes.h>

std::vector<double> ReadWaterTemps(LPCVOID codeCaveAddress, HANDLE hProcess);

bool HookWaterTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);





