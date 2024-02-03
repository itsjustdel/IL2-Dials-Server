#pragma once
#include <windows.h>


bool HookTurnBall(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress);