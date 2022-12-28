#pragma once
#include <Windows.h>
#include <vector>
#include <string>
char originalLineOilTemp[8];

bool Intake(std::string planeName)
{
	std::string v = "FW 190 A3";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isBF109K4(std::string planeName)
{
	std::string v = "Bf 109 K-4";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isBF110E2(std::string planeName)
{
	std::string v = "Bf-110 E2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isBF110G2(std::string planeName)
{
	std::string v = "Bf-110 G2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isHS129B2(std::string planeName)
{
	std::string v = "Hs 129 B-2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

std::vector<double> ReadOilTempsA(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeName) //out
{
	//two engines
	std::vector<double> values(4);
	for (size_t i = 0; i < 4; i++)
	{
		//buffer
		char rawData[sizeof(double)];
		//read address saved in code cave
		LPCVOID targetAddress;
		
		if (isBF110E2(planeName) || isBF110G2(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDF0 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isHS129B2(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD30 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isBF109K4(planeName)) 
		{				
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240 + i * 8), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCD8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}		
		else
		{		
			//pointer +1E0 is offset for water temp in kelvin
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2E0 + i * 8), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x1E0), &rawData, sizeof(double), 0);
			values[i] = *reinterpret_cast<double*>(rawData);
		}
	}

	return values;
}

std::vector<double> ReadOilTempsB(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeName) //in
{
	//two engines
	std::vector<double> values(4);
	for (size_t i = 0; i < 4; i++)
	{
		//buffer
		char rawData[sizeof(double)];
		//read address saved in code cave
		LPCVOID targetAddress;
		if (isBF110E2(planeName) || isBF110G2(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDE0 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else
		{
			//pointer +1E0 is offset for water temp in kelvin
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2E0 + i * 8), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x1E8), &rawData, sizeof(double), 0);
			values[i] = *reinterpret_cast<double*>(rawData);
		}
	}

	return values;
}

bool InjectionOilTemp(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//position in cave where we will start to write
	toCave = (LPVOID)((uintptr_t)(toCave)+0x129);

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineOilTemp, sizeof(originalLineOilTemp), &bytesWritten);//5 is enough for jump plus address

	//0x09 is the byte form of "jmp", assembly language to jump to a location. Note this is a x86 instruction (it can only jump +- 2gb of memory)
	BYTE jump = 0xE9;

	//write jump opcode
	WriteProcessMemory(hProcess, (LPVOID)src, &jump, sizeof(jump), &bytesWritten);
	//work out relative address
	//cave - hook - 5 (jmp)
	//Relative address. Using 32bit data type due to close nature of jump
	uintptr_t relativeAddress = (uintptr_t)toCave - src - 5;
	LPVOID rA = (LPVOID)relativeAddress;
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01), &relativeAddress, sizeof(DWORD), &bytesWritten);

	//we don't need to pad anything, this instruction fits exactly over the previous line

	//we add a nop to pad out memory
	BYTE nops[3] = { 0x90, 0x90, 0x90 };
	//add nops
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}

bool CaveOilTemp(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//position in cave where we will start to write
	toCave = (LPVOID)((uintptr_t)(toCave)+0x129);

	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineOilTemp, sizeof(originalLineOilTemp), &bytesWritten);
	totalWritten += bytesWritten;

	//check for player plane
	uintptr_t relAddress = (uintptr_t)toCave + 0x200 - 0x129;// 0xD1 for where this section of the cave starts	
	//unpack to bytes
	BYTE relBytes[8];
	for (size_t i = 0; i < 8; i++)
		relBytes[i] = relAddress >> (i * 8);

	BYTE bytes[75] = { 0x50,
		// move plane type struct to rax
		0x48, 0xA1, relBytes[0], relBytes[1], relBytes[2], relBytes[3], relBytes[4], relBytes[5], relBytes[6],relBytes[7],
		// cmp rax, r12
		0x4C, 0x39, 0xE0,
		// pop rax
		0x58,
		// jne [addy]
		0x75, 0x3A,
		// cmp r14, 00  //TEST
		0x49, 0x83, 0xFE, 0x00,
		// jne
		0x75, 0x09,
		// rcx to mem
		0x48, 0x89, 0x0D, 0x91, 0x01, 0x00, 0x00,
		// jmp to end
		0xEB, 0x2B,
		// cmp r14, 01
		0x49, 0x83, 0xFE, 0x01,
		// jne [addy]
		0x75, 0x09,
		// rcx to mem
		0x48, 0x89, 0x0D, 0x8A, 0x01, 0x00, 0x00,
		// jmp to end
		0xEB, 0x1C,
		// cmp r14, 02
		0x49, 0x83, 0xFE, 0x02,
		// jne
		0x75, 0x09,
		// rcx to mem
		0x48, 0x89, 0x0D, 0x83, 0x01, 0x00, 0x00,
		// jmp to end
		0xEB, 0x0D,
		// cmp r14, 03
		0x49, 0x83, 0xFE, 0x03,
		// jne
		0x75, 0x07,
		// rcx to mem
		0x48, 0x89, 0x0D, 0x7C, 0x01, 0x00, 0x00
	};

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &bytes, sizeof(bytes), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of function
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 4)));// ...still trial and error for this amount of nops?original line size?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}

bool HookOilTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionOilTemp(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveOilTemp(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}
