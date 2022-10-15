#pragma once
#include <windows.h>
#include <vector>

std::vector<double> ReadOilTempsA(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeName);
std::vector<double> ReadOilTempsB(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeName);

bool HookOilTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);