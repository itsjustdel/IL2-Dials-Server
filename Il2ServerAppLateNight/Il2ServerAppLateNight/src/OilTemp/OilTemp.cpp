#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../PlaneSelector.h"
#include "../Injector/Injector.h"
#include "../RUPlanes/RUPlanes.h"
#include "../GERPlanes/GERPlanes.h"
#include "../UKPlanes/UKPlanes.h"
#include "../USPlanes/USPlanes.h"

char originalLineOilTemp[8];

bool Intake(std::string planeName)
{
	std::string v = "FW 190 A3";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}


std::vector<float> GetLimitsOil(std::string name, boolean isOutOil)
{
	// RU
	if (IsMig3(name)) {
		return isOutOil ? std::vector<float> { 0, 125 } : std::vector<float>{ 0, 120 };
	}
	else if (IsLagg3s29(name) || IsIL2(name) || IsPe2s35(name) || IsLi2(name) || IsI16(name) || IsLa5s8(name) || IsLa5fns2(name) || IsYak169(name) || IsYaks127(name) || IsYak7b36(name) || IsYak9(name) || IsYak9T(name))
	{
		return std::vector<float> { 0, 125 };
	}
	// B
	else if (IsPe2s87(name)) {
		return std::vector<float> { 0, 160 };
	}

	// GER
	// E
	if (IsHs129B2(name) || IsJu88A4(name) || IsJu88C6(name)) {
		return std::vector<float> { 0, 160};
	}
	else if (IsBf110E2(name) || IsBf110G2(name) || IsHe111H6(name) || IsHe111H16(name) || IsJu523mG4E(name)) {
		return std::vector<float> { 0, 120};
	}
	// B D
	else if (IsFW190A3(name) || IsFW190A5(name) || IsFW190A6(name) || IsFW190A8(name)) {
		return std::vector<float> { 0, 125}; //in game needle is a bit off for the fw 190s dial is 0-120 in game
	}
	// A C
	else if (IsBf109E7(name) || IsBf109K4(name) || IsJu87D3(name) || IsME410A1(name))
	{
		return std::vector<float> { 0, 130 };
	}

	// ITA
	if (IsMC202s8(name)) {
		return std::vector<float> { 30, 130 };
	}

	//ROM
	if (IsIAR80(name)) {
		return std::vector<float> { 30, 140 };// to check
	}

	// UK
	if (IsSpitfireMkXIV(name) || IsSpitfireMkXIVe(name)) {
		// B
		return std::vector<float> { 0, 120 };
	}
	else if (IsMosquitoFBMkVIser2(name)) {
		return std::vector<float> { 0, 110 };
	}
	else if (IsUKPlane(name))
	{
		// dial type A
		return std::vector<float> { 0, 100 };
	}

	// US
	if (IsA20B(name)) {
		return std::vector<float> { 20, 120 };
	}

	if (IsP39L(name) || IsP40E(name) || IsP47D22(name)) {
		return std::vector<float> { 0, 100 };
	}
	if (IsP47D28(name)) {
		return std::vector<float> { -70, 150 };
	}

	if (IsP51B5(name)) {
		return std::vector<float> { 20, 76 }; //values checked in debugger		
	}

	if (IsP51D15(name)) {
		return std::vector<float> { -70, 150 };

	}
	if (IsP38(name)) {
		return std::vector<float> { -50, 150 };
	}
	if (IsC47A(name)) {
		return std::vector<float> { -70, 150 };
	}

	return std::vector<float> { 0, 0 };
}

std::vector<float> PercentageConversionOilIn(std::vector<float> percentages, std::string name)
{
	std::vector<float> limits = GetLimitsOil(name, false);
	float range = limits[1] - limits[0];
	for (SIZE_T i = 0; i < 4; i++)
	{
		float p = limits[0] + percentages[i] * range;
		percentages[i] = p;
	}
	return percentages;
}

std::vector<float> PercentageConversionOilOut(std::vector<float> percentages, std::string name)
{
	std::vector<float> limits = GetLimitsOil(name, true);
	float range = limits[1] - limits[0];
	for (SIZE_T i = 0; i < 4; i++)
	{
		float p = limits[0] + percentages[i] * range;
		percentages[i] = p;
	}
	return percentages;
}

std::vector<float> ReadOilTempsIn(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType)
{
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);

	LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

	uintptr_t baseOffset = 0x3e5c;

	for (SIZE_T i = 0; i < 4; i++)
	{
		uintptr_t engineOffset = 0x190 * i;
		uintptr_t offset = baseOffset + (engineOffset);

		//all 2 engine planes have temps next to each other (so far)
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
		const SIZE_T sizeOfData = sizeof(float);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<float*>(rawData);
	}

	return PercentageConversionOilIn(values, planeType);
}

std::vector<float> ReadOilTempsOut(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType)
{
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);

	LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

	uintptr_t baseOffset = 0x3e54;
	for (SIZE_T i = 0; i < 4; i++)
	{
		uintptr_t engineOffset = 0x190 * i;
		uintptr_t offset = baseOffset + (engineOffset);

		//all 2 engine planes have temps next to each other (so far)
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
		const SIZE_T sizeOfData = sizeof(float);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<float*>(rawData);
	}

	return PercentageConversionOilOut(values, planeType);
}

std::vector<float> ReadOilTempsBf109(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType)
{
	//two engines
	std::vector<float> values(4);
	for (SIZE_T i = 0; i < 4; i++)
	{
		//buffer
		char rawData[sizeof(double)];
		//read address saved in code cave
		LPCVOID targetAddress;
		ReadProcessMemory(hProcessIL2, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2E0 + i * 8), &targetAddress, sizeof(LPCVOID), 0);
		ReadProcessMemory(hProcessIL2, (LPCVOID)((uintptr_t)targetAddress + 0x1E0), &rawData, sizeof(double), 0);
		values[i] = *reinterpret_cast<double*>(rawData);
		values[i] -= 273.15f;
	}

	return values;
}



//Outbound
std::vector<double> ReadOilTempsA(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeName)
{
	//two engines
	std::vector<double> values(4);
	for (SIZE_T i = 0; i < 4; i++)
	{
		//buffer
		char rawData[sizeof(double)];
		//read address saved in code cave
		LPCVOID targetAddress;
		if (isP38J(planeName)) {
			// note, same address as "generic"
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xE10 + i * 8), &rawData, sizeof(double), 0);
			double t = *reinterpret_cast<double*>(rawData);
			//t -= 273.15f;
			//additional calcs - RSE.RSE::CAeroplane_P_51B_5::standartCanopySimulation+4338 - F2 0F10 87 500D0000   - movsd xmm0,[rdi+00000D50]
			// newTemp = (((t  + 70) / 220)  //70 and 220 from lookup table
			double temp = (t + 70) / 220;
			// t = (newTemp * 1.95) + 20 // 205 is 1 percent of the range on the dial which is used (-55 to to 150) - needle enver goes over 75? + 20 to add start of range back on after we find percentage
			// percentage in range //what's 0.68 of 195
			t = (temp);

			t = -55 + temp * (205);


			//most planes send temp data in kelvin so adjust now so we have consistency
			t += 273.15;
			values[i] = t;
		}
		else if (isP51B5(planeName)) {
			// note, same address as "generic"
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2E0 + i * 8), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x1E0), &rawData, sizeof(double), 0);
			double t = *reinterpret_cast<double*>(rawData);
			t -= 273.15f;
			//additional calcs - RSE.RSE::CAeroplane_P_51B_5::standartCanopySimulation+4338 - F2 0F10 87 500D0000   - movsd xmm0,[rdi+00000D50]
			// newTemp = (((t  + 70) / 220)  //70 and 220 from lookup table
			double temp = (t + 70) / 220;
			// t = (newTemp * 0.55) + 20 // .55 is 1 percent of the range on the dial which is used (20 to to 75) - needle enver goes over 75? + 20 to add start of range back on after we find percentage
			// percentage in range
			t = ((temp * .55) * 100) + 20;
			//most planes send temp data in kelvin so adjust now so we have consistency
			t += 273.15;
			values[i] = t;
		}
		else if (isP39(planeName)) {
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD78 + i * 8), &rawData, sizeof(double), 0);
			double t = *reinterpret_cast<double*>(rawData);
			//most planes send temp data in kelvin so adjust now so we have consistency
			t += 273.15;
			values[i] = t;
		}
		else if (isMosquito(planeName)) {
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xE18 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isTyphoon(planeName)) {
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDB8 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isTempest(planeName)) {
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD58 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isHurricane(planeName)) {
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD50 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isJU52(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDE0 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isBF110E2(planeName) || isBF110G2(planeName))
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
		else if (isJU88A4(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD98 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isJU88C6(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDA0 + i * 8), &rawData, sizeof(double), 0);
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
			//generic offset from engine struct
			//pointer +1E0 is offset for water temp in kelvin
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x2E0 + i * 8), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x1E0), &rawData, sizeof(double), 0);
			values[i] = *reinterpret_cast<double*>(rawData);
		}
	}

	//adjust ju52 order, organise engines left to right for client
	if (isJU52(planeName)) {
		//values[3] is spare so just use that for the temp
		values[3] = values[1];
		values[1] = values[2];
		values[2] = values[3];
	}

	return values;
}
//Inbound
std::vector<double> ReadOilTempsB(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeName) //in
{
	//two engines
	std::vector<double> values(4);
	for (SIZE_T i = 0; i < 4; i++)
	{
		//buffer
		char rawData[sizeof(double)];
		//read address saved in code cave
		LPCVOID targetAddress;
		if (isJU52(planeName))
		{
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
			ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDC8 + i * 8), &rawData, sizeof(double), 0);
			//most planes send temp data in kelvin so adjust now so we have consistency
			double t = *reinterpret_cast<double*>(rawData);
			t += 273.15;
			values[i] = t;
		}
		else if (isBF110E2(planeName) || isBF110G2(planeName))
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

	//adjust ju52 order, organise engines left to right for client
	if (isJU52(planeName)) {
		//values[3] is spare so jsut use that for the temp
		values[3] = values[1];
		values[1] = values[2];
		values[2] = values[3];
	}

	return values;
}

bool InjectionOilTemp(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//position in cave where we will start to write
	toCave = (LPVOID)((uintptr_t)(toCave)+0x129);

	SIZE_T bytesWritten = 0;
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
	SIZE_T totalWritten = 0;
	//position in cave where we will start to write
	toCave = (LPVOID)((uintptr_t)(toCave)+0x129);

	//cave - where we put our own code alongside the original
	SIZE_T bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineOilTemp, sizeof(originalLineOilTemp), &bytesWritten);
	totalWritten += bytesWritten;

	//check for player plane
	uintptr_t relAddress = (uintptr_t)toCave + 0x200 - 0x129;// 0xD1 for where this section of the cave starts	
	//unpack to bytes
	BYTE relBytes[8];
	for (SIZE_T i = 0; i < 8; i++)
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

bool HookOilTemp(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress)
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
