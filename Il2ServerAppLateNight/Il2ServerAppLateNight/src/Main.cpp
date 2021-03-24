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
#include <Iphlpapi.h>
#include <vector>
#include <string>

#include "Scan.h"
#include "ProcessTools.h"
#include "Injector.h"
#include "Server.h"
#include "IPHelper.h"
#include "PointerToFunction.h"

//how much memory to change permissions on in original code
const int size = 100; //note, min size?

//where we will hold our data from instruments in game
const size_t cockpitValuesLength = 20;
double cockpitValues[cockpitValuesLength];
const size_t altimeterValuesLength = 20;
double altimeterValues[altimeterValuesLength];

//main process flags
bool injectedCockpit;
bool injectedAltimeter;

//functionAddresses
LPCVOID cockpitInstrumentsAddress;
LPCVOID altimeterAddress;

//address of our memory cave we create
LPVOID codeCaveAddress = 0;

//process stuff
wchar_t* exeName = (wchar_t*)L"Il-2.exe";
DWORD processID = 0;
HANDLE hProcessIL2;
MODULEENTRY32 moduleRSE;
bool processFoundCurrent;

float testValues[3];

void CaveRecovered()
{
	injectedCockpit = true;
	injectedAltimeter = true;
	
}

//server uses Gets to grab data before sending it out
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
	//addresses of cave
	codeCaveAddress = 0;

	//reports
	injectedCockpit = false;
	injectedAltimeter = false;
}

 bool GetProcessData()
{
	
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
		

	//RSE.dll
	moduleRSE = GetModule(processID, (wchar_t*)L"RSE.dll");
	if (moduleRSE.dwSize == 0)
		return false;
		

	return true;
}



bool CockpitInstrumentsDataStruct(LPVOID toCockpitInstruments) 
{
	//we need to read the data and get what values we need from it
	//need to finalise numbers
	const size_t sizeOfData = cockpitValuesLength * 8;
	char rawData[sizeOfData];
	ReadProcessMemory(hProcessIL2, toCockpitInstruments, &rawData, sizeOfData, NULL);
	
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
	//injection saves cockpit pointer at code cave's address + 0x60
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x60);
	LPVOID toCockpitInstruments = PointerToDataStruct(hProcessIL2, addressToRead );

	if (toCockpitInstruments != 0)
	{		
		CockpitInstrumentsDataStruct(toCockpitInstruments);
		return 1;
	}
	 
	return 0;
}


bool ReadAltimeter()
{

	//keep reading this value as if we change plane it can change
	//injection saves alt. pointer at code cave's address + 0x80
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x80);
	LPVOID toAltimeter = PointerToDataStruct(hProcessIL2, addressToRead);
	
	if (toAltimeter != 0)
	{
		AltimeterDataStruct(toAltimeter);
		return 1;
	}

	return 0;
}


void ReadTest()
{
	//we represent the data with floats in the app, so let's convert now and save network traffic
	float floatArray[3];
	//read memory only when requested
	ReadCockpitInstruments();
	ReadAltimeter();

	//if we have found the altimeter struct we can read from here, this allows us to get the needle position as it moves so we don't need to calculate that ourselves
	floatArray[0] = GetAltitude();
	//mgh
	floatArray[1] = GetMMHg();
	//airspeed
	floatArray[2] = GetAirspeedFromCockpitStruct();
}

int Injector(System::ComponentModel::BackgroundWorker^ worker)
{	
	auto lastChecked = std::chrono::system_clock::now();
	//put this time in the past so first check fires instantly
	lastChecked -= std::chrono::hours(1);
	
	while (true)
	{

		bool test = true;
		if (test)
		{

			altimeterValues[5] += 1;

			altimeterValues[8] += 1;

			cockpitValues[14] += 1;

			ReadTest();

			Sleep(1);
			continue;
		}


		//check if game is running 
		//after found once, only check this every so often, causes too much cpu usage
		//if we haven't received anything, start a timout timer
		//check for 5 seconds no receive
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

		//find getPointerToCockpitinstruments location, if we haven't found already
		if (cockpitInstrumentsAddress == 0)
		{
			cockpitInstrumentsAddress = PointerToFunction("getPointerToCockpitInstruments", hProcessIL2, moduleRSE);
			if (cockpitInstrumentsAddress == 0)
			{
				worker->ReportProgress(1);
				continue;
			}
		}


		//find getPointerToAltimeter location, if we haven't found already
		if (altimeterAddress == 0)
		{
			altimeterAddress = PointerToFunction("getPointerToAltimeter", hProcessIL2, moduleRSE);
			if (altimeterAddress == 0)
			{
				worker->ReportProgress(2);
				continue;
			}
		}

		//create or recover code cave	
		if (codeCaveAddress == 0)
		{
			if (!CodeCave(hProcessIL2, (uintptr_t)cockpitInstrumentsAddress, moduleRSE, codeCaveAddress))
			{
				worker->ReportProgress(3);
				continue;
			}
		}

		//inject cockpit		
		if (!injectedCockpit)
		{
			injectedCockpit = HookCockpitInstruments(hProcessIL2, (void*)(cockpitInstrumentsAddress), size, codeCaveAddress);
			if (!injectedCockpit)
			{
				//Hook function overwrites original code and writes to our code cave
				worker->ReportProgress(4);
				continue;
			}

			//note: //could create pointer for injected cockpit and asign within if check to clean this up further
		}

		//inject altimeter		
		if (!injectedAltimeter)
		{
			injectedAltimeter = HookAltimeter(hProcessIL2, (void*)(altimeterAddress), size, codeCaveAddress);
			if (!injectedAltimeter)
			{
				//Hook function overwrites original code and writes to our code cave
				worker->ReportProgress(5);
				continue;
			}
		}

		//we got here, good, tell the interface
		worker->ReportProgress(6);


		
	



		//don't need a fast cycle on this loop
		Sleep(100);
	}

	return 0;
}