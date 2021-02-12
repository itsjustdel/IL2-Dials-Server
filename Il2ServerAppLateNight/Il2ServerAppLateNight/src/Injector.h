#pragma once
#include <Windows.h>
#include <TlHelp32.h>


bool Injection(HANDLE hProcess, uintptr_t src, LPVOID toCave);

bool CaveCockpitInstruments(HANDLE hProcess, uintptr_t src, LPVOID toCave);

bool CaveAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave);

LPVOID AllocateMemory(HANDLE hProcess, uintptr_t src);

LPVOID AllocateMemoryUp(HANDLE hProcess, uintptr_t src);

bool HookCockpitInstruments(HANDLE hProcess, void* pSrc, size_t size, LPVOID toCave);

bool HookAltimeter(HANDLE hProcess, void* pSrc, size_t size, LPVOID toCave);

bool RestoreOriginalMemory(HANDLE hProcess, LPVOID AOBresult, LPVOID caveStart, SIZE_T size, char *originalMemory);

bool SaveOriginalMemory(HANDLE hProcess, LPVOID AOBresult, SIZE_T size, char *outStr);

LPVOID PointerToDataStruct(HANDLE hProcess, LPVOID caveStart);

