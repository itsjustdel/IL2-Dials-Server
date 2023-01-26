#pragma once
#include <windows.h>

bool HookDynamicBody(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);