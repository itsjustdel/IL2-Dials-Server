#pragma once
#include <wtypes.h>

bool HookDynamicBody(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);