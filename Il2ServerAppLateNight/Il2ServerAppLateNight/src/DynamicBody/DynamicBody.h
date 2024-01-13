#pragma once
#include <windows.h>

bool HookDynamicBody(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);