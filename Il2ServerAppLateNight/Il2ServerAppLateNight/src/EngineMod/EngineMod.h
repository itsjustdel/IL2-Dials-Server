#pragma once
#include <windows.h>

bool HookEngineModification(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);