#pragma once
#include <windows.h>

bool HookPlaneType(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);
