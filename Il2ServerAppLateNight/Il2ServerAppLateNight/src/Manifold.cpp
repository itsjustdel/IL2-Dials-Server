#include "Manifold.h"
#include "injector.h"
#include <Windows.h>
#include <vector>
#include <string>

char originalLineManifold[8];

std::vector<double> GermanManifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2)
{
	std::vector<double> manifoldValues(4);
	for (size_t i = 0; i < 4; i++)
	{
		//offset in cave, four addresses to read for each plane
		//first engine is + 0x180 from cave, 2nd 0x188..etc
		uintptr_t offset = 0x08 * i;
		LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x280 + offset);
		LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

		LPVOID _manifold = (LPVOID)((uintptr_t)(toStruct)+0x9F8);
		const size_t sizeOfData = sizeof(double);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, _manifold, &rawData, sizeOfData, NULL);

		manifoldValues[i] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}

std::vector<double> USManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	
	std::vector<double> manifoldValues(4);
	
	
	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);

	std::string v = "P-38J-25";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDD8), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDE0), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);	
	}

	v = "A-20B";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD40), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD48), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);
	}
	
	v = "P-40E-1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD48), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-39L-1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD48), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}
		
	v = "P-47D-28";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD18), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}
	
	v = "P-51D-15";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-47D-22";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD08), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-51B-5";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD30), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}


std::vector<double> UKManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	std::vector<double> manifoldValues(4);

	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
		

	std::string v = "Spitfire Mk.IXe";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD30 + 0x38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.Vb";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCF8 + 0x38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Tempest Mk.V ser.2";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD40), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Hurricane Mk.II";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.XIV";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xE88), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Typhoon Mk.Ib";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDA0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}

std::vector<double> RUManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	std::vector<double> manifoldValues(4);

	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);


	std::string v = "Yak-7B ser.36";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xC98), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);		
	}

	v = "Yak-9 ser.1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xC98), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);		
	}

	v = "Yak-9T ser.1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCA0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);		
	}

	//bring in line with german manifold scale
	manifoldValues[0] *= 100;

	return manifoldValues;
}


bool CaveManifold(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//read address from rcx and save in codecave
	toCave = (LPVOID)((uintptr_t)(toCave)+0x6A);
	size_t totalWritten = 0;
	size_t bytesWritten = 0;
	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineManifold, sizeof(originalLineManifold), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;

	//first of all we need to check if this is indeed the call for the player plane
	//if r13 matches the player plane address, we are good to go
	//compare r13 and rax
	//push - save temp var
	BYTE push[1] = { 0x50 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), push, sizeof(push), &bytesWritten);
	totalWritten += bytesWritten;

	//this is absolute mov instruction for rax
	BYTE movToRax[2] = { 0x48, 0xA1 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), movToRax, sizeof(movToRax), &bytesWritten);
	totalWritten += bytesWritten;

	//now it will want an address to mov to rax	 (this is where plane type address is stored)
	uintptr_t relativeAddress = (uintptr_t)toCave + 0x200 - 0x6A;// 0x76 for where this section of the cave starts
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &relativeAddress, sizeof(LPCVOID), &bytesWritten);
	totalWritten += bytesWritten;

	//compare r13 and rax
	BYTE cmp[3] = { 0x4C, 0x39, 0xE8 };
	//0x08 will be relative jump to code cave plus 0x100
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmp, sizeof(cmp), &bytesWritten);
	totalWritten += bytesWritten;

	//pop rax back now we're done - restore temp var
	BYTE pop[1] = { 0x58 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), pop, sizeof(pop), &bytesWritten);
	totalWritten += bytesWritten;

	//if not player plane, return 
	BYTE jumpIfNotEqual[2] = { 0x75, 0x34 }; //0x34 is relative jump to return line
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	//there can be mroe than one engine, engines are indexed in rsi 
	//1DE53290007 - 48 83 FF 00
	BYTE rsiEqualToZero[4] = { 0x48, 0x83, 0xFE, 0x00 };
	// if not equal, do another check for the next engine index
	BYTE rsiEqualToOne[4] = { 0x48, 0x83, 0xFE, 0x01 };
	//and engine no 3?
	BYTE rsiEqualToTwo[4] = { 0x48, 0x83, 0xFE, 0x02 };
	//and 4?! - I don't want to do have to figure this out again!
	BYTE rsiEqualToThree[4] = { 0x48, 0x83, 0xFE, 0x03 };

	//if equal, this will set a flag to be checked on..
	//jump short is 75, next byte is relative address
	//redefine 2nd element to in jump to 0x07
	//
	jumpIfNotEqual[1] = 0x07;





	//write these	
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToZero, sizeof(rsiEqualToZero), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xD0, 0x01, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToOne, sizeof(rsiEqualToOne), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	rcxToMem[3] = 0xCB;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToTwo, sizeof(rsiEqualToTwo), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	rcxToMem[3] = 0xC6;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToThree, sizeof(rsiEqualToThree), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	rcxToMem[3] = 0xC1;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of function
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 4)));// ...still trial and error for this amount of nops?original line size?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	//1D1AAB1AF67
	return 1;
}


bool InjectionManifold(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	toCave = (LPVOID)((uintptr_t)(toCave)+0x6A);
	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineManifold, sizeof(originalLineManifold), &bytesWritten);

	BYTE jump = 0xE9;

	//write jump opcode
	WriteProcessMemory(hProcess, (LPVOID)src, &jump, sizeof(jump), &bytesWritten);

	//Relative address. Using 32bit data type due to close nature of jump
	uintptr_t relativeAddress = (uintptr_t)toCave - src - 5;
	LPVOID rA = (LPVOID)relativeAddress;
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01), &relativeAddress, sizeof(DWORD), &bytesWritten);

	//we need to add a nope to pad out memory so we jump back at same point we left
	BYTE nops[3] = { 0x90, 0x90, 0x90 };
	//add nops
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}


bool HookManifold(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionManifold(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveManifold(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}