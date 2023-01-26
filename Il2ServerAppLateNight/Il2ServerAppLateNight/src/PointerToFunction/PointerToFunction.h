#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

LPCVOID PointerToFunction(std::string functionName, HANDLE hProcessIL2, MODULEENTRY32 moduleRSE);
