#pragma once
#include <windows.h>
#include <vector>

std::vector<float> ReadOilTempsIn(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType);
std::vector<float> ReadOilTempsOut(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType);
std::vector<float> ReadOilTempsBf109(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType);
bool HookOilTemp(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);