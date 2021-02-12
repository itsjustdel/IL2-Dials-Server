#pragma once
#include <Windows.h>

char * PatternScan(char * pStart, UINT_PTR RegionSize, const char * szPattern, const char * szMask);
char * PatternScanEx(HANDLE hProc, char * pStart, UINT_PTR RegionSize, const char * szPattern, const char * szMask);

template <class T>
UINT_PTR ScanMemory(char * pData, UINT_PTR RegionSize, T Val, UINT_PTR * pOut, UINT_PTR MaxCount, BYTE Alignment, char * OriginalBase = nullptr)
{
	UINT_PTR Count = 0;
	for (ULONG i = 0; i < (RegionSize - sizeof(T)) && Count < MaxCount; i += Alignment, pData += Alignment)
		if (*reinterpret_cast<T*>(pData) == Val)
		{
			if (OriginalBase)
				pOut[Count] = reinterpret_cast<UINT_PTR>(OriginalBase + i);
			else
				pOut[Count] = reinterpret_cast<UINT_PTR>(pData);
			Count++;
		}
	return Count;
}

template <class T>
UINT_PTR ScanMemoryEx(HANDLE hProc, char * pStart, UINT_PTR RegionSize, T Val, UINT_PTR * pOut, UINT_PTR MaxCount, BYTE Alignment)
{
	UINT_PTR Count = 0;
	DWORD Buffer = 0;
	if (!GetHandleInformation(hProc, &Buffer))
		return 0;

	char * pCurrent = pStart;
	while (pCurrent <= pStart + RegionSize - Alignment && Count < MaxCount)
	{
		MEMORY_BASIC_INFORMATION MBI{ 0 };
		if (!VirtualQueryEx(hProc, pCurrent, &MBI, sizeof(MEMORY_BASIC_INFORMATION)))
			return Count;

		if (MBI.State == MEM_COMMIT && !(MBI.Protect & PAGE_NOACCESS))
		{
			if (pCurrent + MBI.RegionSize > pStart + RegionSize)
				MBI.RegionSize = pStart + RegionSize - pCurrent;

			char * Data = new char[MBI.RegionSize];

			if (!Data)
				return Count;

			if (ReadProcessMemory(hProc, pCurrent, Data, MBI.RegionSize, nullptr))
			{
				UINT_PTR NewResults = ScanMemory(Data, MBI.RegionSize, Val, &pOut[Count], MaxCount - Count, Alignment, pCurrent);
				Count += NewResults;
			}
			delete[] Data;
		}
		pCurrent += MBI.RegionSize;
	}

	return Count;
}