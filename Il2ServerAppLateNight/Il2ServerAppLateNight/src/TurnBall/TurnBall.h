#pragma once
#include <windows.h>


bool HookTurnBall(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);