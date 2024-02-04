#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../Injector/Injector.h"
#include "../USPlanes/USPlanes.h"

std::vector<float> GetLimitsCarbAirMix(std::string name)
{
	// none - p40

	// A p47d-22  -70 - 150
	if (IsP47D22(name)) return { -70, 150 };
	// B p47D-28
	if (IsP47D28(name)) return { -70, 150 };
	// C p51d15, p51b-5
	if (IsP51B5(name) || IsP51D15(name)) return { -70, 150 };
	// D A20
	if (IsA20B(name)) return { -45, 45 };
	// E c-47, p38
	if (IsC47A(name) || IsP38(name)) return { -70, 150 };
	// F p39
	if (IsP39L(name)) {
		return { -50, 50 };
	}


	return std::vector<float> { 0, 0 };
}

std::vector<float> PercentageConversionCarbAirMix(std::vector<float> percentages, std::string name)
{
	std::vector<float> limits = GetLimitsCarbAirMix(name);
	float range = limits[1] - limits[0];
	for (size_t i = 0; i < 4; i++)
	{
		float p = limits[0] + percentages[i] * range;
		percentages[i] = p;
	}
	return percentages;
}

std::vector<float> CarbAirMixTemps(LPVOID codeCaveAddress, HANDLE hProcess, std::string planeType)
{
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);
	LPVOID toStruct = PointerToDataStruct(hProcess, addressToRead);
	for (SIZE_T i = 0; i < 4; i++)
	{
		uintptr_t engineOffset = 0x190 * i;
		uintptr_t offset = 0x3dbc + (engineOffset);
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
		const size_t sizeOfData = sizeof(float);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcess, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<float*>(rawData);
	}


	return PercentageConversionCarbAirMix(values, planeType);
}