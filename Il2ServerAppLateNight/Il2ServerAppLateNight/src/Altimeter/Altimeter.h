#pragma once
#include <windows.h>

bool HookAltimeter(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);
