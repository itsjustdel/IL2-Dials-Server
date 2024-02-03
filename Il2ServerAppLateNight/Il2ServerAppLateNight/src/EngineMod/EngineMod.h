#pragma once
#include <windows.h>

bool HookEngineModification(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);