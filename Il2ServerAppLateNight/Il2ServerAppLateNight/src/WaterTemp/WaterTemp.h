#pragma once
#include <windows.h>
#include <vector>
#include <string>

std::vector<float> WaterTemps(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeType);

bool HookWaterTemp(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);
