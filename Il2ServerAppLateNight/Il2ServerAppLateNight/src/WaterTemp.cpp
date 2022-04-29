#pragma once
#include <Windows.h>
#include <vector>

char originalLineWaterTemp[8];

std::vector<double> ReadWaterTemps(HANDLE hProcess, LPVOID codeCaveAddress)
{
	//two engines
	std::vector<double> values(2);
	for (size_t i = 0; i < 2; i++)
	{
		//buffer
		char rawData[sizeof(double)];
		//read address saved in code cave
		LPCVOID targetAddress;
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2C0 + i*8), &targetAddress, sizeof(LPCVOID), 0);

		//pointer +178 is offset for water temp in kelvin
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x178), &rawData, sizeof(double), 0);
		values[i] = *reinterpret_cast<double*>(rawData);
	}

	return values;
}

bool InjectionWaterTemp(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//position in cave where we will start to write
	toCave = (LPVOID)((uintptr_t)(toCave)+0xD1);

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineWaterTemp, sizeof(originalLineWaterTemp), &bytesWritten);//5 is enough for jump plus address

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

bool CaveWaterTemp(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//position in cave where we will start to write
	toCave = (LPVOID)((uintptr_t)(toCave)+0xD1);

	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineWaterTemp, sizeof(originalLineWaterTemp), &bytesWritten);
	totalWritten += bytesWritten;

	//r14 has the engine number (up to 2) - will have to review for 3 or 4 engine plane. The ju52 doesn't have water temps (a 3 engine plane)
	//compare and jump if not equal
	BYTE cmpR14ToZero[4] = { 0x49, 0x83, 0xFE, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmpR14ToZero, sizeof(cmpR14ToZero), &bytesWritten);
	totalWritten += bytesWritten;

	BYTE jumpEngineOneA[2] = { 0x75, 0x0F };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpEngineOneA, sizeof(jumpEngineOneA), &bytesWritten);
	totalWritten += bytesWritten;

	BYTE cmpR9ToZero[4] = { 0x49, 0x83, 0xF9, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmpR9ToZero, sizeof(cmpR9ToZero), &bytesWritten);
	totalWritten += bytesWritten;

	BYTE jumpEngineOneB[2] = {  0x75, 0x1C };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpEngineOneB, sizeof(jumpEngineOneB), &bytesWritten);
	totalWritten += bytesWritten;
	
	//if equal, write rcx to mem
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xD4, 0x01, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//and jump to exit
	BYTE jumpToEndA[2] = { 0xEB, 0x13 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpToEndA, sizeof(jumpToEndA), &bytesWritten);
	totalWritten += bytesWritten;

	//if we didn't jump
	BYTE cmpR14ToOne[4] = { 0x49, 0x83, 0xFE, 0x01 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmpR14ToOne, sizeof(cmpR14ToOne), &bytesWritten);
	totalWritten += bytesWritten;

	//and jump to exit if not equal
	BYTE jumpToEndB[2] = { 0x75, 0x0D };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpToEndB, sizeof(jumpToEndB), &bytesWritten);
	totalWritten += bytesWritten;

	//cmp r9 to zero again - re-use byte array
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmpR9ToZero, sizeof(cmpR9ToZero), &bytesWritten);
	totalWritten += bytesWritten;

	BYTE jumpEngineTwo[2] = { 0x75, 0x07 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpEngineTwo, sizeof(jumpEngineTwo), &bytesWritten);
	totalWritten += bytesWritten;

	//if true, we write this, if false we jump over this
	BYTE rcxToMemSecondEngine[7] = { 0x48, 0x89, 0x0D, 0xC7, 0x01, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMemSecondEngine, sizeof(rcxToMemSecondEngine), &bytesWritten);
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

bool HookWaterTemp(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionWaterTemp(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveWaterTemp(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}
