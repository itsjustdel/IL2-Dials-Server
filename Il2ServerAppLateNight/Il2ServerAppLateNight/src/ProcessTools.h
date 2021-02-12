#pragma once
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcID(wchar_t* exeName);
MODULEENTRY32 GetModule(DWORD dwProcID, wchar_t* moduleName);
double ElapsedTimeInSeconds(HANDLE hProcess);
