#pragma once
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcID(wchar_t* exeName)
{

	PROCESSENTRY32 procEntry = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!hSnapshot)
	{
		return 0;
	}

	procEntry.dwSize = sizeof(procEntry);

	if (!Process32First(hSnapshot, &procEntry))
	{
		return 0;
	}

	do
	{
		if (!wcscmp(procEntry.szExeFile, exeName))
		{
			CloseHandle(hSnapshot);
			return procEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &procEntry));

	CloseHandle(hSnapshot);
	return 0;
}

MODULEENTRY32 GetModule(DWORD dwProcID, wchar_t* moduleName)
{
	MODULEENTRY32 modEntry = { 0 };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);

	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 curr = { 0 };

		curr.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &curr))
		{
			do
			{
				if (!wcscmp(curr.szModule, moduleName))
				{
					modEntry = curr;
					break;
				}
			} while (Module32Next(hSnapshot, &curr));
		}
		CloseHandle(hSnapshot);
	}
	return modEntry;
}

double ElapsedTimeInSeconds(HANDLE hProcess)
{
	FILETIME createTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;

	GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime);
	SYSTEMTIME readableTimeCreation;
	FileTimeToSystemTime(&createTime, &readableTimeCreation);

	SYSTEMTIME readableTimeUser;
	FileTimeToSystemTime(&createTime, &readableTimeUser);

	SYSTEMTIME currentTimeSys;
	GetSystemTime(&currentTimeSys);
	FILETIME currentFileTime;
	SystemTimeToFileTime(&currentTimeSys, &currentFileTime);

	// get time here
	ULARGE_INTEGER start;
	start.LowPart = createTime.dwLowDateTime;
	start.HighPart = createTime.dwHighDateTime;

	ULARGE_INTEGER current;
	current.LowPart = currentFileTime.dwLowDateTime;
	current.HighPart = currentFileTime.dwHighDateTime;

	ULARGE_INTEGER elapsed;
	elapsed.QuadPart = current.QuadPart - start.QuadPart;


	return elapsed.QuadPart * 1e-7; //turns to seconds
}