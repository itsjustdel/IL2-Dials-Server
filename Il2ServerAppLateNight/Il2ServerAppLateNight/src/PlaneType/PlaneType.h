#pragma once
#include <windows.h>

bool HookPlaneType(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);
