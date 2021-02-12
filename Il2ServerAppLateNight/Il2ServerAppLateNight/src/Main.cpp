#pragma once
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <Psapi.h>
#include <cstdlib>
#include <chrono>
//#include <future>
#include <Iphlpapi.h>
#include <vector>
#include <string>

#include "Scan.h"
#include "ProcessTools.h"
#include "Injector.h"
#include "Server.h"
#include "IPHelper.h"
//#include "../Form1.h"


//how much memory to change permissions on in original code
const int size = 100; //note, doesnt' seem to need this, keeping anyway

const size_t cockpitValuesLength = 20;
double cockpitValues[cockpitValuesLength];
double altimeterValues[20];

//main process flags
bool injectedCockpit;
bool injectedAltimeter;

int injectReport = -1;

//original memory - not using - i'lm keep the commented code 
//char originalMemoryCI[size];
//char originalMemoryA[size];

//aob results
char* AOBResultCockpitInstruments;
char* AOBResultAltimeter;

//address of our memory we create
LPVOID cockpitInstrumentsCaveStart = 0;
LPVOID altimeterCaveStart = 0;

//process stuff
wchar_t* exeName = (wchar_t*)L"Il-2.exe";
DWORD processID = 0;
HANDLE hProcessIL2;
MODULEENTRY32 moduleIL2;
MODULEENTRY32 moduleRSE;

//status flags
bool processFoundPrevious;
bool processFoundCurrent;
//debug reports
bool aobReportedCI;
bool aobReportedA;
bool injectionReportedCockpit;
bool injectionReportedAltimeter;
bool toCockpitReported;
bool toAltimeterReported;


extern "C" __declspec(dllexport) int GetInjectedCockpit()
{
	return injectReport;
}

extern "C" __declspec(dllexport) intptr_t GetAOBResultCockpitInstruments()
{
	return (intptr_t)AOBResultCockpitInstruments;
}

double GetAltitude()
{
	return altimeterValues[5];//check
}

double GetMMHg()
{
	return altimeterValues[8];
}

double GetAirspeedFromCockpitStruct()
{
	return cockpitValues[14];
}

void ResetFlags()
{
	//addresses of caves
	cockpitInstrumentsCaveStart = 0;
	altimeterCaveStart = 0;

	//addresses of where we inject
	AOBResultCockpitInstruments = 0;
	AOBResultAltimeter = 0;

	//reports
	injectedCockpit = false;
	injectedAltimeter = false;

	//debug reports
	aobReportedCI = false;
	aobReportedA = false;
	injectionReportedCockpit = false;
	injectionReportedAltimeter = false;
	toCockpitReported = false;
	toAltimeterReported = false;
}

extern "C" __declspec(dllexport) bool GetProcessData()
{
	//track changes for debug
	processFoundPrevious = processFoundCurrent;

	//Get Process ID by enumerating the processes using tlhelp32snapshot	
	processID = GetProcID(exeName);

	//return if we didn't find the process	
	if (processID == 0)
	{
		//make sure everything is reset, can happen if game resets but app continues
		ResetFlags();

		return false;
	}


	//Get handle by OpenProcess
	hProcessIL2 = OpenProcess(PROCESS_ALL_ACCESS, false, processID); //PROCESS_ALL_ACCESS needed to create code cave
	if (hProcessIL2 == 0)
		return false;

	//IL2 main exe

	moduleIL2 = GetModule(processID, (wchar_t*)L"Il-2.exe");
	if (moduleIL2.dwSize == 0)
		return false;

	//RSE.dll


	moduleRSE = GetModule(processID, (wchar_t*)L"RSE.dll");
	if (moduleRSE.dwSize == 0)
		return false;
		

	return true;
}

extern "C" __declspec(dllexport) intptr_t AOBScanCockpitInstruments()
{
	//aob scanner char arrays to search for address	

	const char* szPattern = "\x48\x89\x6C\x24\x68\x8D\x68\x0C\x48\x89\x83\xFC\x02\x00\x00";
	const char* szMask = "?xxxxxxx?xxxxxx";//? tests

	//set scanner to look at the start of il-2.exe
	char* pStart = (char*)moduleIL2.modBaseAddr;
	//only search withing this region
	UINT_PTR regionSize = moduleIL2.modBaseSize;//dwsize

	
	//scan and return address where we want to inject
	AOBResultCockpitInstruments = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (AOBResultCockpitInstruments == 0)
		return false;
	else
	{
		//debug message
		if (!aobReportedCI)
		{
			std::cout << "AOB Found Cockpit instruments" << std::endl;
			aobReportedCI = true;
		}
	}

	return (intptr_t)AOBResultCockpitInstruments;
}

bool AOBScanAltimeter()
{
	//aob scanner char arrays to search for address	
	
	const char* szPattern = "\xF2\x0F\x10\x44\x24\x60\xE8\xF4\x78\xF2\xFF";
	const char* szMask = "?xxxxx?xxxx";


	//set scanner to look at the start of il-2.exe
	char* pStart = (char*)moduleRSE.modBaseAddr;
	//only search withing this region
	UINT_PTR regionSize = moduleRSE.modBaseSize;


	//scan and return address where we want to inject
	AOBResultAltimeter = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (AOBResultAltimeter == 0)
		return 0;
	else
	{
		//debug message
		if (!aobReportedA)
		{
			std::cout << "AOB Found Altimeter" << std::endl;
			aobReportedA = true;
		}
	}

	return 1;
}

bool AOBScanMySignatureCockpitInstruments()
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
	char* pStart = (char*)(moduleIL2.modBaseAddr);// +moduleRSE.modBaseSize);
	//only search withing this region
	UINT_PTR regionSize = 2147483648;///2gb

	//scan and return address where we want to inject
	char* sigResult = PatternScanEx(hProcessIL2, pStart, regionSize, szPattern, szMask);

	if (sigResult == 0)
	{
		std::cout << "ERROR - No Aob Results Cockpit Signature" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Cockpit instruments code cave recovered" << std::endl;

		//we recovered our lost cave address but must move address back x to get to the start
		cockpitInstrumentsCaveStart = (LPVOID(uintptr_t(sigResult) - 0x20)); //note 0x20 is the same as 32 (no 0x"")

		//we can set flags as if we wrote the injection
		injectedCockpit= true;
		AOBResultCockpitInstruments = (char*)0xDEL;//this just needs to be non zero - if we really need the first aob scan result addrewss we can read the cave return address
	}

	return 1;
}

bool AOBScanMySignatureAltimeter()
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
		std::cout << "ERROR - No Aob Results Altiemter Signature" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Altimeter code cave recovered" << std::endl;

		//we recovered our lost cave address but must move address back x to get to the start
		altimeterCaveStart = (LPVOID(uintptr_t(sigResult)-0x20)); //note 0x20 is the same as 32 (no 0x"")

		//we can set flags as if we wrote the injection
		injectedAltimeter = true;
		AOBResultAltimeter =  (char*)0xDEL;//this just needs to be non zero - if we really need the first aob scan result addrewss we can read the cave return address
	}

	return 1;
}

LPVOID AllocateMemoryL(HANDLE hProcess, uintptr_t src)
{
	//to return, the address where we found the memory
	LPVOID toCave = 0;

	//find unallocated memory
	MEMORY_BASIC_INFORMATION mbi;

	for (SIZE_T addr = (SIZE_T)src; addr > (SIZE_T)src - 0x80000000; addr = (SIZE_T)mbi.BaseAddress - 1)
	{
		if (!VirtualQueryEx(hProcess, (LPCVOID)addr, &mbi, sizeof(mbi)))
		{
			break;
		}
		//scan through until FREE block is discovered
		if (mbi.State == MEM_FREE)
		{
			if (toCave = VirtualAllocEx(hProcess, mbi.BaseAddress, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))//64kb size, smallest granulation/and write privelages need to be kept open
			{

				return toCave;// break;
			}
		}
	}

	return 0;
}


extern "C" __declspec(dllexport) int InjectCockpitInstruments()
{
	//Find some unused memory within the target module/exe and allocate for ourselves

	cockpitInstrumentsCaveStart = AllocateMemoryUp(hProcessIL2, (uintptr_t)(AOBResultCockpitInstruments));
	
	if (cockpitInstrumentsCaveStart == 0)
	{
		injectReport = 0;
		return 0;
	}


	//	//Hook function overwrites original code and writes to our code cave
	if (!HookCockpitInstruments(hProcessIL2, (void*)(AOBResultCockpitInstruments), size, cockpitInstrumentsCaveStart))//100? size of read memory
	{
		injectReport = 1;
		return 1;
	}


	if (!injectionReportedCockpit)
	{
		std::cout << "Injection complete - Cockpit Instruments" << std::endl;
		injectionReportedCockpit = true;
	}


	injectReport = 2;
	return 2;
	
}


bool InjectAltimeter()
{
	//Find some unused memory within the target module/exe and allocate for ourselves
	altimeterCaveStart = AllocateMemoryUp(hProcessIL2, (uintptr_t)(moduleRSE.modBaseAddr));
	if (altimeterCaveStart == 0)
		return 0;

	//Hook function overwrites original code and writes to our code cave
	if (!HookAltimeter(hProcessIL2, (void*)(AOBResultAltimeter), size, altimeterCaveStart))//100? size of read memory
		return 0;

	if (!injectionReportedAltimeter)
	{
		std::cout << "Injection complete - Altimeter" << std::endl;
		injectionReportedAltimeter = true;
	}

	return true;

}

bool CockpitInstrumentsDataStruct(LPVOID toCockpitInstruments) 
{
	//we need to read the data and get what values we need from it
	//need to finalise numbers
	const size_t sizeOfData = cockpitValuesLength * 8;
	char rawData[sizeOfData];
	ReadProcessMemory(hProcessIL2, toCockpitInstruments, &rawData, sizeOfData, NULL);

	//double cockpitDoubles[cockpitDoublesLength];
	
	//step through data read 8 bytes at a time and grab double
	size_t dCount = 0;
	for (size_t i = 0; i < sizeOfData; i+=8, dCount++)//8 bytes to get double (?)
	{
		char temp[8];
		for (size_t j = 0; j < 8; j++)
		{
			temp[j] = rawData[i + j];
		}

		//interpret byte array as double and put in array
		cockpitValues[dCount] = *reinterpret_cast<double*>(temp);
	}

	//fail state?

	return 1;
}

bool AltimeterDataStruct(LPVOID structStart)
{
	
	//we need to read the data and get what values we need from it
	//need to finalise numbers
	const size_t sizeOfData = 10 * 8;
	char rawData[sizeOfData];
	ReadProcessMemory(hProcessIL2, structStart, &rawData, sizeOfData, NULL);

		

	//step through data read 8 bytes at a time and grab doubles -- could go directly to [7] if i work out the maths
	size_t dCount = 0;
	for (size_t i = 0; i < sizeOfData; i += 8, dCount++)//8 bytes to get double (?)
	{
		char temp[8];
		for (size_t j = 0; j < 8; j++)
		{
			temp[j] = rawData[i + j];
		}

		//interpret byte array as double and put in array
		altimeterValues[dCount] = *reinterpret_cast<double*>(temp);
	}


	return 1;
}



bool ReadCockpitInstruments()
{

	//keep reading this value as if we change plane it can change
	LPVOID toCockpitInstruments = PointerToDataStruct(hProcessIL2,  cockpitInstrumentsCaveStart);

	if (toCockpitInstruments != 0)
	{
		if (!toCockpitReported)
		{
			std::cout << "Cockpit pointer found" << std::endl;
			toCockpitReported = true;
		}

		CockpitInstrumentsDataStruct(toCockpitInstruments);

		return 1;
	}
	else return 0;


}

bool ReadAltimeter()
{

	//keep reading this value as if we change plane it can change
	LPVOID toAltimeter = PointerToDataStruct(hProcessIL2, altimeterCaveStart);

	//we only find altimeter on reference pressure toggle
	if (toAltimeter != 0)
	{
		if (!toAltimeterReported)
		{
			std::cout << "Altimeter pointer found" << std::endl;
			toAltimeterReported = true;
		}

		AltimeterDataStruct(toAltimeter);

		return 1;
	}
	else return 0;

}



int CockpitInstruments()
{
	//find the area of code to patch and patch
		//what if INJECTION  already there? would happen on app restart but game continue/NEEDED**********
	if (AOBResultCockpitInstruments == 0)
	{
		if (AOBScanCockpitInstruments())
		{
			//Let's save original memory at aobscan result location if we find anything
			if (AOBResultCockpitInstruments != nullptr)
			{
				//saving memory at aob scan result address
				//SaveOriginalMemory(hProcessIL2, (LPVOID)AOBResultCockpitInstruments, size, originalMemoryCI);

				//write our own instruction to read register
				injectedCockpit = InjectCockpitInstruments();

				return 1;
			}
			else
				return 2;
		}
		else
		{
			//if aob scan doesn't find the original pattern, perhaps we already injected and user restarted server
			//so let's look for our own hook

			std::cout << "Original Cockpit Instruments code changed, Looking for signature..." << std::endl;
			if (AOBScanMySignatureCockpitInstruments())
			{
				return 3;
			}
			else
			{
				return 4;
			}
		}
	}
	else
		return 5;

	//read data structs! -called from server.cpp now upon client request
//	if (injectedCockpit)
//		ReadCockpitInstruments();
}

int Altimeter()
{

	if (AOBResultAltimeter == 0)
	{
		if (AOBScanAltimeter())
		{
			if (AOBScanAltimeter != nullptr)
			{
				//SaveOriginalMemory(hProcessIL2, (LPVOID)AOBResultAltimeter, size, originalMemoryA);

				injectedAltimeter = InjectAltimeter();

				return 1 + 10;
			}
			else
				return 2 + 10;
		}
		else
		{
			//if aob scan doesn't find the original pattern, perhaps we already injected and user restarted server
			//so let's look for our own hook

			std::cout << "Original Altimeter code changed, Looking for signature..." << std::endl;
			if (AOBScanMySignatureAltimeter())
			{
				return 3 + 10;
			}
			else 
				return 4 + 10;
		}
	}
	else
		return 5 + 10;



	//if (injectedAltimeter) //server calls this
	//	ReadAltimeter();

}

/*
int cantorFunction(int k1, int k2) {
	int sum = k1 + k2;
	int triangularNumberOfSum = triangularNumber(sum);
	return (tringularNumberOfSum + k2);
}
*/

int ReadData(System::ComponentModel::BackgroundWorker^ worker)
{
	
	auto lastChecked = std::chrono::system_clock::now();
	//put this time in the past so first check fires instantly
	lastChecked -= std::chrono::hours(1);
	
	while (true)
	{
		//check if game is running 
		//after found once, only check this every so often, causes too muchcpu usage
		//if we haven't received anything, start a timout timer
		
		//check for 5 secodns no receive
		auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastChecked).count();
		if (timeInSeconds > 5)
		{	
			//look for game process
			processFoundCurrent = GetProcessData();
			

			//reset last checked 
			lastChecked = std::chrono::system_clock::now();
		}

		//if we don't find our target exe
		if (!processFoundCurrent)
		{
			//using BackgroundWorker class to send info safely cross threaded
			worker->ReportProgress(0);
	
			//can't spam getprocessdata, too much cpu usage
			Sleep(1000);

			//std::cout << "Waiting for exe" << std::endl;
			continue;
		}

		//Main method general instrument panel
		int progressCockpit = CockpitInstruments();
		//add ten to altimiter result so we can work out a cominbed report
		int progressAltimeter = Altimeter();

		int combinedProgress = progressCockpit + progressAltimeter;

		worker->ReportProgress(combinedProgress);


		Sleep(100);
	}

	return 0;
}

extern "C" __declspec(dllexport) void Instruments() 
{	
	// methods to read data //called externally 
	CockpitInstruments();
	Altimeter();
}


