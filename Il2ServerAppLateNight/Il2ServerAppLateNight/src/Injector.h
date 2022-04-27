#pragma once
#include <Windows.h>
#include <TlHelp32.h>

bool CodeCave(HANDLE hProcess, uintptr_t src, MODULEENTRY32 moduleRSE, LPVOID& codeCaveAddress);

LPVOID AllocateMemory(HANDLE hProcess, uintptr_t src);

LPVOID AllocateMemoryUp(HANDLE hProcess, uintptr_t src);

bool RestoreOriginalMemory(HANDLE hProcess, LPVOID AOBresult, LPVOID caveStart, SIZE_T size, char *originalMemory);

bool SaveOriginalMemory(HANDLE hProcess, LPVOID AOBresult, SIZE_T size, char *outStr);

LPVOID PointerToDataStruct(HANDLE hProcess, LPVOID caveStart);

