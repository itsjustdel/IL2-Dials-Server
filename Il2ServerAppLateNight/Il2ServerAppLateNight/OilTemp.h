#pragma once
#include <windows.h>
#include <vector>

std::vector<double> ReadOilTemps(HANDLE hProcess, LPVOID codeCaveAddress);

bool HookOilTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);