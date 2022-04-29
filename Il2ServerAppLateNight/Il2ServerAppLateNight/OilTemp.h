#pragma once
#include <windows.h>

bool HookOilTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);