#pragma once
#include <windows.h>

bool HookAltimeter(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);
