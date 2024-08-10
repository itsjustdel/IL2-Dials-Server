#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../Injector/Injector.h"
#include "../RUPlanes/RUPlanes.h"
#include "../GERPlanes/GERPlanes.h"
#include "../UKPlanes/UKPlanes.h"
#include "../USPlanes/USPlanes.h"

char originalLineWaterTemp[8];

bool isGerWaterD(std::string name) {
	return IsBf109F4(name) || IsBf109G2(name) || IsBf109F2(name) || IsBf109F4(name) || IsBf109G6(name) || IsBf109G6Late(name) || IsBf109G6AS(name);
}

std::vector<float> GetLimitsWaterTemp(std::string name)
{
	// RU
	// 0 - 120 A
	// lagg3 s29, il2 41,42,43, mig3 s24, pe s35, li2
	if (IsLagg3s29(name) || IsIL2(name) || IsMig3(name) || IsPe2s35(name) || IsLi2(name))
	{
		return std::vector<float> { 0, 120 };
	}

	// 0 - 125 B
	// yak 1 s69, yak 7b s36, yak 9s1, yak9t s1
	if (IsYak169(name) || IsYak7b36(name) || IsYak9(name) || IsYak9T(name))
	{
		return std::vector<float> { 0, 125 };
	}

	// 0 - 160 C
	// pe 2 s87
	if (IsPe2s87(name))
	{
		return std::vector<float> { 0, 160 };
	}

	//none la-5 s8, i 16, la5fn s2

	// GER
	// A
	// j87d3, bf109e7, bf109g14, me410a1
	// B
	// he11h6, bf110e2, ju88a4, bf110g2, he11h16, ju88c6
	// C
	// bf109k4, fw190d9
	// D
	// f4, g2, f2, g4, g6, g6-late, bf109g6as
	// 
	// Note: IAR ?

	if (IsJu87D3(name) || IsBf109E7(name) || IsBf109G14(name) || IsME410A1(name))
	{
		// A
		return std::vector<float> { 0, 130};
	}

	else if (IsHe111H6(name) || IsHe111H16(name) || IsBf110E2(name) || IsBf110G2(name) || IsJu88A4(name) || IsJu88C6(name))
	{
		// B
		return std::vector<float> { 0, 120};
	}

	if (IsBf109K4(name) || IsFW190D9(name)) {
		// C
		return std::vector<float> { 0, 130};
	}

	// combo dial
	if (isGerWaterD(name)) {
		// D
		return std::vector<float> { 0, 80};
	}

	// ITA
	if (IsMC202s8(name)) {
		return std::vector<float> { 0, 130 };
	}

	// UK 
	if (IsSpitfireMkXIV(name))
		return std::vector<float> { 40, 140 };
	else if (IsUKPlane(name))
	{
		return std::vector<float> { 0, 140 };
	}

	// US
	// A 50 - 15
	// p-40e1, p-39l-1
	if (IsP40E(name) || IsP39L(name)) {
		return std::vector<float> {50, 150};
	}

	// B -70, 150
	// P38J-25
	if (IsP38(name)) {
		return std::vector<float> {-70, 150};
	}

	// C -70, 150
	// P-51D-15, P-51B-5
	if (IsP51B5(name) || IsP51D15(name)) {
		return std::vector<float> {-70, 150};
	}
	// none p-47d-28, P-47D-22, C-47A, a20


	return std::vector<float> { 0, 135 }; // UK A is 140, GER A is 130 -- to do FC planes
}

std::vector<float> PercentageConversionWater(std::vector<float> percentages, std::string name)
{
	std::vector<float> limits = GetLimitsWaterTemp(name);
	float range = limits[1] - limits[0];
	for (size_t i = 0; i < 4; i++)
	{
		float p = limits[0] + percentages[i] * range;
		percentages[i] = p;
	}
	return percentages;
}
std::vector<float> WaterTemps(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeType)
{
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);

	LPVOID toStruct = PointerToDataStruct(hProcess, addressToRead);

	bool gerWaterD = isGerWaterD(planeType);
	for (size_t i = 0; i < 4; i++)
	{
		if (gerWaterD) {
			// bf 109 raw water value - This dial seems to be on a different data point, 
			// but we read the oil directly too, since it is a double dial so will leave this way for now
			char rawData[sizeof(double)];
			//read address saved in code cave
			LPCVOID targetAddress;
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2C0 + i * 8), &targetAddress, sizeof(LPCVOID), 0);

			//pointer +178 is offset for water temp in kelvin
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x178), &rawData, sizeof(double), 0);

			values[i] = *reinterpret_cast<double*>(rawData);
			values[i] -= 273.15f; // kelvin to C
		}
		else {
			//use draw arg value
			uintptr_t engineOffset = 0x190 * i;
			uintptr_t offset = 0x3ea4 + 0x8 + (engineOffset);
			LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
			const size_t sizeOfData = sizeof(float);
			char rawData[sizeOfData];
			ReadProcessMemory(hProcess, temp, &rawData, sizeOfData, NULL);

			values[i] = *reinterpret_cast<float*>(rawData);
		}
	}

	// don't convert ger water d raw data values
	if (gerWaterD) return values;

	return PercentageConversionWater(values, planeType);
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

	uintptr_t relAddress = (uintptr_t)toCave + 0x200 - 0xD1;// 0xD1 for where this section of the cave starts	
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
		// cmp r14, 00
		0x49, 0x83, 0xFE, 0x00,
		// jne
		0x75, 0x09,
		// rcx to mem
		0x48, 0x89, 0x0D, 0xC9, 0x01, 0x00, 0x00,
		// jmp to end
		0xEB, 0x2B,
		// cmp r14, 01
		0x49, 0x83, 0xFE, 0x01,
		// jne [addy]
		0x75, 0x09,
		// rcx,[addy]
		0x48, 0x89, 0x0D, 0xC2, 0x01, 0x00, 0x00,
		// jmp to end
		0xEB, 0x1C,
		// cmp r14, 02
		0x49, 0x83, 0xFE, 0x02,
		// jne
		0x75, 0x09,
		// rcx to mem
		0x48, 0x89, 0x0D, 0xBB, 0x01, 0x00, 0x00,
		// jmp to end
		0xEB, 0x0D,
		// cmp r14, 03
		0x49, 0x83, 0xFE, 0x03,
		// jne
		0x75, 0x07,
		// rcx to mem
		0x48, 0x89, 0x0D, 0xB4, 0x01, 0x00, 0x00,
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
