//Brohoin's memory stuff
#pragma once
#include "Scan.h"

bool INT_ComparePattern(char * szSource, const char * szPattern, const char * szMask)
{
	for (; *szMask; ++szSource, ++szPattern, ++szMask)
		if (*szMask == 'x' && *szSource != *szPattern)
			return false;
	return true;
}

char * INT_PatternScan(char * pData, UINT_PTR RegionSize, const char * szPattern, const char * szMask, int Len)
{
	for (UINT i = 0; i != RegionSize - Len; ++i, ++pData)
		if (INT_ComparePattern(pData, szPattern, szMask))
			return pData;
	return nullptr;
}

char * PatternScan(char * pStart, UINT_PTR RegionSize, const char * szPattern, const char * szMask)
{
	char * pCurrent = pStart;
	auto Len = lstrlenA(szMask);

	while (pCurrent <= pStart + RegionSize - Len)
	{
		MEMORY_BASIC_INFORMATION MBI{ 0 };
		if (!VirtualQuery(pCurrent, &MBI, sizeof(MEMORY_BASIC_INFORMATION)))
			return nullptr;

		if (MBI.State == MEM_COMMIT && !(MBI.Protect & PAGE_NOACCESS))
		{
			if (pCurrent + MBI.RegionSize > pStart + RegionSize - Len)
				MBI.RegionSize = pStart + RegionSize - pCurrent + Len;

			char * Ret = INT_PatternScan(pCurrent, MBI.RegionSize, szPattern, szMask, Len);

			if (Ret)
				return Ret;
		}
		pCurrent += MBI.RegionSize;
	}

	return nullptr;
}

char * PatternScanEx(HANDLE hProc, char * pStart, UINT_PTR RegionSize, const char * szPattern, const char * szMask)
{
	DWORD Buffer = 0;
	if (!GetHandleInformation(hProc, &Buffer))
		return nullptr;

	char * pCurrent = pStart;
	auto Len = lstrlenA(szMask);

	while (pCurrent <= pStart + RegionSize - Len)
	{
		MEMORY_BASIC_INFORMATION MBI{ 0 };
		if (!VirtualQueryEx(hProc, pCurrent, &MBI, sizeof(MEMORY_BASIC_INFORMATION)))
			return nullptr;

		bool state = MBI.State == MEM_COMMIT;
		bool mbiProtection = (MBI.Protect & PAGE_NOACCESS);
		//if (state && !mbiProtection)
		if (MBI.State == MEM_COMMIT && !(MBI.Protect & PAGE_NOACCESS))
		{
			UINT_PTR Delta = pCurrent - reinterpret_cast<char*>(MBI.BaseAddress);
			MBI.RegionSize -= Delta;

			if (pCurrent + MBI.RegionSize > pStart + RegionSize - Len)
				MBI.RegionSize -= pCurrent + MBI.RegionSize - pStart - RegionSize + Len;

			char * Data = new char[MBI.RegionSize];
			if (!ReadProcessMemory(hProc, pCurrent, Data, MBI.RegionSize, nullptr))
			{
				delete[] Data;
				pCurrent = pCurrent + MBI.RegionSize;
				continue;
			}

			//exit if we get to here - something not right with this function, had to add this
			if (MBI.RegionSize == 0)
				return nullptr;

			char * Ret = INT_PatternScan(Data, MBI.RegionSize, szPattern, szMask, Len);
			delete[] Data;

			if (Ret)
				return (Ret - Data + pCurrent);
		}

		pCurrent = pCurrent + MBI.RegionSize;
	}

	return nullptr;
}

