//Brohoin's memory stuff
#pragma once
#include "Scan.h"
#include <TlHelp32.h>
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



#pragma region Unused AOB methods
//UNUSED
intptr_t AOBScanCockpitInstruments(HANDLE hProcessIL2, MODULEENTRY32 moduleRSE)
{
	//aob scanner char arrays to search for address	

	//const char* szPattern = "\x48\x89\x6C\x24\x68\x8D\x68\x0C\x48\x89\x83\xFC\x02\x00\x00";
	//48 8B 81 00 09 00 00 C3 CC CC CC CC CC CC CC CC
	const char* szPattern = "\x48\x8B\x81\x00\x09\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	const char* szMask = "?xxxxxxxxxxxxxxx";

	/*
	//set scanner to look at the start of il-2.exe
	char* pStart = (char*)moduleIL2.modBaseAddr;
	//only search withing this region
	UINT_PTR regionSize = moduleIL2.modBaseSize;//dwsize
	*/
	//rse.dll
	char* pStart = (char*)moduleRSE.modBaseAddr;
	//only search withing this region
	UINT_PTR regionSize = moduleRSE.modBaseSize;


	//scan and return address where we want to inject
	char* AOBResultCockpitInstruments = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (AOBResultCockpitInstruments == 0)
		return false;
	else
	{
		/*
		//debug message
		if (!aobReportedCI)
		{
			std::cout << "AOB Found Cockpit instruments" << std::endl;
			aobReportedCI = true;
		}
		*/
	}

	return (intptr_t)AOBResultCockpitInstruments;
}

//UNUSED
bool AOBScanAltimeter(HANDLE hProcessIL2, MODULEENTRY32 moduleRSE)
{


	//aob scanner char arrays to search for address	

	const char* szPattern = "\xF2\x0F\x10\x44\x24\x60\xE8\xF4\x78\xF2\xFF";
	const char* szMask = "?xxxxx?xxxx";


	//set scanner to look at the start of rse
	char* pStart = (char*)moduleRSE.modBaseAddr;
	//only search withing this region
	UINT_PTR regionSize = moduleRSE.modBaseSize;


	//scan and return address where we want to inject
	char* AOBResultAltimeter = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (AOBResultAltimeter == 0)
		return 0;


	return 1;
}



bool AOBScanMySignatureCockpitInstruments(HANDLE hProcessIL2, MODULEENTRY32 moduleIL2)
{

	//search for code cave that was previously hooked. We get here if server is force quit so game keeps code
	//if server is quit, hook is unwritten (still to do)
	//this function is very slow, around a second for a successful match, and a few more for an failure, but it will happen so rarely I'm not looking to improve on it

	//"DellyWellyCockpit in hex [17]	
	const char* szPattern = "\x44\x65\x6c\x6c\x79\x57\x65\x6c\x6c\x79\x43\x6f\x63\x6b\x70\x69\x74";
	const char* szMask = "xxxxxxxxxxxxxxxxx";


	//allocate memory for code cave looks BEFORE the modules so we need to start looking for our code cave before
	char* pStart = (char*)(moduleIL2.modBaseAddr);// +moduleRSE.modBaseSize);
	//only search withing this region
	UINT_PTR regionSize = 2147483648;///2gb

	//scan and return address where we want to inject
	char* sigResult = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (sigResult == 0)
	{
		//std::cout << "ERROR - No Aob Results Cockpit Signature" << std::endl;
		return 0;
	}
	else
	{
		//std::cout << "Cockpit instruments code cave recovered" << std::endl;

		//we recovered our lost cave address but must move address back x to get to the start
		LPVOID codeCaveAddress = (LPVOID(uintptr_t(sigResult) - 0x40)); //0x40 after cavestart is where we hold the sig

		//we can set flags as if we wrote the injection
		//injectedCockpit = true;
		char* AOBResultCockpitInstruments = (char*)0xDEL;//this just needs to be non zero - if we really need the first aob scan result addrewss we can read the cave return address
	}

	return 1;
}

bool AOBScanMySignatureAltimeter(HANDLE hProcessIL2, MODULEENTRY32 moduleRSE)
{

	//search for code cave that was previously hooked. We get here if server is force quit so game keeps code
	//if server is quit, hook is unwritten (still to do)
	//this function is very slow, around a second for a successful match, and a few more for an failure, but it will happen so rarely I'm not looking to improve on it

	//"DellyWellyAltimeter " + 0x90
	//BYTE mySig[20] = { 0x44, 0x65, 0x6c, 0x6c, 0x79, 0x57, 0x65, 0x6c, 0x6c, 0x79, 0x41, 0x6c, 0x74, 0x69, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x90 };
						//44     65    6C    6C    79    57    65    6C    6C    79    41    6C    74    69    6D    65    74    65    72    00
	const char* szPattern = "\x44\x65\x6c\x6c\x79\x57\x65\x6c\x6c\x79\x41\x6c\x74\x69\x6d\x65\x74\x65\x72\x00";
	const char* szMask = "xxxxxxxxxxxxxxxxxxxx";


	//allocate memory for code cave looks BEFORE the modules so we need to start looking for our code cave before
	char* pStart = (char*)(moduleRSE.modBaseAddr);// +moduleRSE.modBaseSize);
	//only search withing this region
	UINT_PTR regionSize = 2147483648;///2gb

	//scan and return address where we want to inject
	char* sigResult = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (sigResult == 0)
	{
		//std::cout << "ERROR - No Aob Results Altiemter Signature" << std::endl;
		return 0;
	}
	else
	{
		//std::cout << "Altimeter code cave recovered" << std::endl;

		//we recovered our lost cave address but must move address back x to get to the start
		LPVOID codeCaveAddress = (LPVOID(uintptr_t(sigResult) - 0x20)); //note 0x20 is the same as 32 (no 0x"")

		//we can set flags as if we wrote the injection
		//injectedAltimeter = true;
		char* AOBResultAltimeter = (char*)0xDEL;//this just needs to be non zero - if we really need the first aob scan result addrewss we can read the cave return address
	}

	return 1;
}

#pragma endregion
