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
#include "USPlanes.h"
//#include "TurnNeedle.h"
#include "../TurnNeedle.h"
#include "Server.h"
#include "IPHelper.h"
#include "PointerToFunction.h"
#include <sstream>



float version = 0.4f;

//how much memory to change permissions on in original code
const int size = 100; //note, min size?

//where we will hold our data from instruments in game
const size_t cockpitValuesLength = 100;
double cockpitValues[cockpitValuesLength];
const size_t altimeterValuesLength = 20;
double altimeterValues[altimeterValuesLength];
double turnNeedleValue;
double turnBallValue;
//where we hold planeTpye string
std::string planeType;


//main process flags
//bool injectedCockpit;
bool injectedAltimeter;
bool injectedPlaneType;
bool injectedTurnNeedle;
bool injectedTurnBall;

//functionAddresses
//LPCVOID cockpitInstrumentsAddress;
LPCVOID altimeterAddress;
LPCVOID setPlayerPresenceAddress;
LPCVOID turnNeedleAddress;
LPCVOID turnBallAddress;

//address of our memory cave we create
LPVOID codeCaveAddress = 0;

//process stuff
wchar_t* exeName = (wchar_t*)L"Il-2.exe";
DWORD processID = 0;
HANDLE hProcessIL2;
MODULEENTRY32 moduleRSE;
bool processFoundCurrent;

void CaveRecovered()
{
	//injectedCockpit = true;
	injectedAltimeter = true;	
	injectedPlaneType = true;
	injectedTurnNeedle = true;
	injectedTurnBall = true;
}

//server uses Gets to grab data before sending it out

bool GetInjected()
{
	if (injectedAltimeter && injectedPlaneType && injectedTurnNeedle && injectedTurnNeedle)
		return true;
	else	
		return false;	
}


float GetIL2DialsVersion()
{
	return version;
}

std::string GetPlaneType()
{
	return planeType;
}

double GetMMHg()
{
	return altimeterValues[8];
}

double GetAltitude()
{
	return altimeterValues[5];
}

double GetAirspeedFromCockpitStruct()
{
	return cockpitValues[14];
}

double GetHeading()
{
	return cockpitValues[3];
}

double GetPitch()
{
	return cockpitValues[4];
}

double GetRoll()
{
	return cockpitValues[5];
}
double GetVSI()
{
	return cockpitValues[27];
}

double GetTurnAndBankBall()
{
	return turnBallValue;
}

double GetTurnAndBankNeedle()
{
	return turnNeedleValue;
}

double GetRPM(int engine)
{
	return cockpitValues[31 + engine];
}



void ResetFlags()
{
	//addresses of cave
	codeCaveAddress = 0;
	//cockpitInstrumentsAddress = 0;
	altimeterAddress = 0;
	setPlayerPresenceAddress = 0;
	//reports
	//injectedCockpit = false;
	injectedAltimeter = false;
	injectedPlaneType = false;
	injectedTurnNeedle = false;
	injectedTurnBall = false;
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



bool CockpitInstrumentsDataStruct(LPVOID structStart) 
{
	//set player presence gives us the address we passed at "structStart"
	//The pointer to the cockpit insturments struct is at structStart + 0x60
	

	//we need to read the data and get what values we need from it
	//need to finalise numbers
	const size_t sizeOfData = cockpitValuesLength * 8;
	char rawData[sizeOfData];
	ReadProcessMemory(hProcessIL2, structStart, &rawData, sizeOfData, NULL);
	
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


bool PlaneTypeDataStruct(LPVOID structStart)
{
	//string starts at 7b from pointer 

	LPVOID addressToRead = (LPVOID)((uintptr_t)(structStart)+0x7B);

	//read string stored at "structStart"
	char rawData[64];
	ReadProcessMemory(hProcessIL2, addressToRead, &rawData, 64, NULL);
	std::string planeName;

	for (size_t i = 0; i < 64; i++)
	{	
		//look for null terminated string
		if (rawData[i] == 0x00)
			break;

		planeName.push_back(rawData[i]);

	}

	planeType = planeName;

	return 1;
}


bool ReadPlaneType()
{

	//injection saves alt. pointer at code cave's address + 0x100
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x100);
	LPVOID toPlaneType = PointerToDataStruct(hProcessIL2, addressToRead);
	if (toPlaneType!= 0)
	{
		PlaneTypeDataStruct(toPlaneType);
	}
	return 1;
	

	return 0;
}


bool ReadCockpitInstruments()
{

	//injection saves cockpit pointer at code cave's address + 0x100 ( this is from setPlayerPresence - but it ahs the address we need)
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x100);
	LPVOID toPlaneTypeStruct = PointerToDataStruct(hProcessIL2, addressToRead );
	LPVOID toCockpitInstruments = PointerToDataStruct(hProcessIL2, (LPVOID)((uintptr_t)(toPlaneTypeStruct) + 0x60));

	if (toCockpitInstruments != 0)
	{		
		CockpitInstrumentsDataStruct(toCockpitInstruments);
		return 1;
	}
	 
	return 0;
}

bool ReadAltimeter()
{


	//injection saves alt. pointer at code cave's address + 0xxx
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x120);
	LPVOID toAltimeter = PointerToDataStruct(hProcessIL2, addressToRead);
	
	if (toAltimeter != 0)
	{
		AltimeterDataStruct(toAltimeter);
		return 1;
	}

	return 0;
}

bool ReadTurnNeedle()
{

	//How to find new offsets-
	//Find value in debugger by ising +140 offset in cave. Needle is clamped number after NaNs e.g 24, 31
	//watch value and check offsets of found instructions (or use turn needle scanner!)

	//injection saves alt. pointer at code cave's address + 0xxx
	//pointer start of struct in our cave
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x140);
	//read
	LPVOID toDynamicBodyStruct = PointerToDataStruct(hProcessIL2, addressToRead);

	uintptr_t offset = OffsetToTurnNeedle(planeType);

	uintptr_t target = (uintptr_t)(toDynamicBodyStruct)+offset;
	
	LPVOID toTurnNeedle = (LPVOID)(target); 

	const size_t sizeOfData = sizeof(double);
	char rawData[sizeOfData];
	ReadProcessMemory(hProcessIL2, toTurnNeedle, &rawData, sizeOfData, NULL);

	//re interporet raw to double
	turnNeedleValue = *reinterpret_cast<double*>(rawData);


	//some are mirrored
	if (planeType.compare("Yak-7B ser.36") == 0 )
		turnNeedleValue *= -1;

	if (planeType.compare("Tempest Mk.V ser.2") == 0)
		turnNeedleValue *= -1;
	
	if (planeType.compare("Spitfire Mk.IXe") == 0)	
		turnNeedleValue *= -1;

	if (planeType.compare("Hurricane Mk.II") == 0)
		turnNeedleValue *= -1;

	if (planeType.compare("Yak-9 ser.1") == 0)
		turnNeedleValue *= -1;

	if (planeType.compare("Yak-9T ser.1") == 0)
		turnNeedleValue *= -1;
	

	return 0;
}


bool ReadTurnCoordinatorBall()
{


	//injection saves pointer at code cave's address + 0xxx
	//pointer start of struct in our cave
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x160);
	//read
	LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);
	//needle value at +D0
	LPVOID toTurnBall = (LPVOID)((uintptr_t)(toStruct)+0xD0);

	const size_t sizeOfData = sizeof(double);
	char rawData[sizeOfData];
	ReadProcessMemory(hProcessIL2, toTurnBall, &rawData, sizeOfData, NULL);

	//re interporet raw to double
	turnBallValue = *reinterpret_cast<double*>(rawData);

	return 0;
}




void ReadTest()
{
	//we represent the data with floats in the app, so let's convert now and save network traffic
	float floatArray[9];
	//read memory only when requested
	ReadCockpitInstruments();
	ReadAltimeter();
	ReadPlaneType();
	ReadTurnNeedle();
	ReadTurnCoordinatorBall();

	//if we have found the altimeter struct we can read from here, this allows us to get the needle position as it moves so we don't need to calculate that ourselves
	floatArray[0] =(float)(GetAltitude());
	//mgh
	floatArray[1] = (float)(GetMMHg());
	//airspeed
	floatArray[2] = (float)(GetAirspeedFromCockpitStruct());
	//heading
	floatArray[3] = (float)(GetHeading());
	//Pitch
	floatArray[4] = (float)(GetPitch());
	//Roll
	floatArray[5] = (float)(GetRoll());
	//vertical speed 
	floatArray[6] = (float)(GetVSI());
	//ball
	floatArray[7] = (float)(GetTurnAndBankBall());
	//needle t and b
	floatArray[8] = (float)(GetTurnAndBankNeedle());

}

void SendTest()
{
	for (size_t i = 0; i < cockpitValuesLength; i++)
	{
		cockpitValues[i] += 0.0000001;
	}
}

int FindFunctions(System::ComponentModel::BackgroundWorker^ worker)
{
	//get plane name - must be done at start of mission
	if (setPlayerPresenceAddress == 0)
	{
		setPlayerPresenceAddress = PointerToFunction("setPlayerPresence", hProcessIL2, moduleRSE);
		if (setPlayerPresenceAddress == 0)
		{
			worker->ReportProgress(1);

			return 0;
		}
	}

	//find getPointerToAltimeter location, if we haven't found already
	if (altimeterAddress == 0)
	{
		altimeterAddress = PointerToFunction("getPointerToAltimeter", hProcessIL2, moduleRSE);
		if (altimeterAddress == 0)
		{
			worker->ReportProgress(2);
			return 0;
		}
	}


	//turn and bank needle seems to be in "DynamicBody" section in RSE.dll


	//find "AF0" by manually locating needle numbers (IL2 clamped at 24 degrees(for il2 1941) and finding relative address
	if (turnNeedleAddress == 0)
	{
		//CCockpitInstruments::simulation is full name after compilation but name is scrambled slightly in dll export list
		//Any function with eg. RSE::exampleNameSpace::ExampleMethod needs to be reformatted like this
		//change :: for @ and flip
		// https://www.codeproject.com/Articles/28969/HowTo-Export-C-classes-from-a-DLL - for an example in a table

		std::string str("simulation@CCockpitInstruments");
		turnNeedleAddress = PointerToFunction(str, hProcessIL2, moduleRSE);
		if (turnNeedleAddress == 0)
		{
			worker->ReportProgress(3);

			return 0;
		}
	}

	if (turnBallAddress == 0)
	{

		//CAccelerationBallIndicator::simulation is full name after compilation but name is scrambled slightly in dll export list
		//Any function with eg. RSE::exampleNameSpace::ExampleMethod needs to be reformatted like this
		//change :: for @ and flip
		// https://www.codeproject.com/Articles/28969/HowTo-Export-C-classes-from-a-DLL - for an example in a table

		std::string str("simulation@CAccelerationBallIndicator");
		turnBallAddress = PointerToFunction(str, hProcessIL2, moduleRSE);
		if (turnBallAddress == 0)
		{
			worker->ReportProgress(3);

			return 0;
		}
	}

	return 1;
}

int FindCodeCave(System::ComponentModel::BackgroundWorker^ worker)
{

	//create or recover code cave	
	if (codeCaveAddress == 0)
	{
		if (!CodeCave(hProcessIL2, (uintptr_t)setPlayerPresenceAddress, moduleRSE, codeCaveAddress))
		{
			worker->ReportProgress(4);
			return 0;
		}
	}

	return 1;
}

int Injections(System::ComponentModel::BackgroundWorker^ worker) 
{

	
	//inject getplanetype
	if (!injectedPlaneType)
	{
		injectedPlaneType = HookPlaneType(hProcessIL2, (void*)(setPlayerPresenceAddress), size, codeCaveAddress);
		if (!injectedPlaneType)
		{
			//Hook function overwrites original code and writes to our code cave
			worker->ReportProgress(5);
			return 0;
		}
	}


	//inject altimeter		
	if (!injectedAltimeter)
	{
		injectedAltimeter = HookAltimeter(hProcessIL2, (void*)(altimeterAddress), size, codeCaveAddress);
		if (!injectedAltimeter)
		{
			//Hook function overwrites original code and writes to our code cave
			worker->ReportProgress(6);
			return 0;
		}
	}


	//inject turn needle
	if (!injectedTurnNeedle)
	{
		injectedTurnNeedle = HookTurnNeedle(hProcessIL2, (void*)(turnNeedleAddress), size, codeCaveAddress);
		if (!injectedTurnNeedle)
		{
			//Hook function overwrites original code and writes to our code cave
			worker->ReportProgress(7);
			return 0;
		}
	}

	//inject turn ball
	if (!injectedTurnBall)
	{
		injectedTurnBall = HookTurnBall(hProcessIL2, (void*)(turnBallAddress), size, codeCaveAddress);
		if (!injectedTurnBall)
		{
			//Hook function overwrites original code and writes to our code cave
			worker->ReportProgress(8);
			return 0;
		}
	}


	return 1;

}

int NeedleScan(System::ComponentModel::BackgroundWorker^ worker)
{
	//debugging function to find offsets
	LPCVOID needleOffset = TurnNeedleScanner(turnNeedleAddress, hProcessIL2, injectedTurnNeedle, codeCaveAddress, hProcessIL2);
	if (needleOffset != 0)
		//convert to int and send
		worker->ReportProgress((uintptr_t)needleOffset);

	return 1;
}

void ClearAddresses()
{
	setPlayerPresenceAddress = 0;
	altimeterAddress = 0;
	turnNeedleAddress = 0;
	turnBallAddress = 0;
}

int Injector(System::ComponentModel::BackgroundWorker^ worker)
{	
	auto lastChecked = std::chrono::system_clock::now();
	//put this time in the past so first check fires instantly
	lastChecked -= std::chrono::hours(1);

	
	
	while (true)
	{
		//false data
		//SendTest();
		//continue;


		//check if game is running 
		//after found once, only check this every so often, causes too much cpu usage
		//if we haven't received anything, start a timout timer
		//check for 5 seconds no receive
		auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastChecked).count();
		//have a seperate timer for process ID check, in case we want to check slower than the Sleep function at the bottom of the while loop
		if (timeInSeconds > .1f)
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


			//clear addresses in case il2 is restarting, there will be all new addresses after restart
			ClearAddresses();

			//can't spam getprocessdata, too much cpu usage
			Sleep(1000);

			//std::cout << "Waiting for exe" << std::endl;
			continue;
		}

		//scan for functions and save addresses		
		if (FindFunctions(worker) == 0)
			//if all functions not found, skip(we have reported back to worker thread already
			continue;

		//Code Cave
		if (FindCodeCave(worker) == 0)
			continue;

		//Inject code
		if (Injections(worker) == 0)
			continue;
		
		//debugging function
		//NeedleScan(worker);

		//we got here, good, tell the interface
		worker->ReportProgress(9); //--change messageErrorLimit variable in Form1.h if this changes

		//don't need a fast cycle on this loop
		Sleep(100);
	}

	return 0;
}