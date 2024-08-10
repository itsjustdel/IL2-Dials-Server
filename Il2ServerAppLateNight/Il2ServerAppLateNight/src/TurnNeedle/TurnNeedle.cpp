#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../Injector/Injector.h"
#include "../USPlanes/USPlanes.h"

std::vector<float> GetLimitsTurnNeedle(std::string name)
{
	return std::vector<float> { 0, 1 }; // 6 degress per second standard turn? --as is in game sending atm
}

float PercentageConversionTurnNeedle(float percentage, std::string name)
{
	std::vector<float> limits = GetLimitsTurnNeedle(name);
	float range = limits[1] - limits[0];
	return limits[0] + percentage * range;
}

float TurnNeedleValue(HANDLE hProcess, LPVOID codeCaveAddress, std::string planeType)
{
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);
	LPVOID toStruct = PointerToDataStruct(hProcess, addressToRead);
	uintptr_t offset = 0x8a1c + 0x8;
	LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
	const size_t sizeOfData = sizeof(float);
	char rawData[sizeOfData];
	ReadProcessMemory(hProcess, temp, &rawData, sizeOfData, NULL);

	float value = *reinterpret_cast<float*>(rawData);

	return PercentageConversionTurnNeedle(value, planeType);
}
