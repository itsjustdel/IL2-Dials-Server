#pragma once
#include <Windows.h>
#include <TlHelp32.h>



bool CodeCave(HANDLE hProcess, uintptr_t src, MODULEENTRY32 moduleRSE, LPVOID& codeCaveAddress);

bool CaveCockpitInstruments(HANDLE hProcess, uintptr_t src, LPVOID toCave);

bool CaveAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave);

LPVOID AllocateMemory(HANDLE hProcess, uintptr_t src);

LPVOID AllocateMemoryUp(HANDLE hProcess, uintptr_t src);

bool HookCockpitInstruments(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool HookAltimeter(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool HookPlaneType(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool HookTurnNeedle(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool HookTurnBall(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool HookManifold(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool HookEngineModification(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);

bool RestoreOriginalMemory(HANDLE hProcess, LPVOID AOBresult, LPVOID caveStart, SIZE_T size, char *originalMemory);

bool SaveOriginalMemory(HANDLE hProcess, LPVOID AOBresult, SIZE_T size, char *outStr);

LPVOID PointerToDataStruct(HANDLE hProcess, LPVOID caveStart);

