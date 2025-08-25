#include <Windows.h>
#include <vector>
#include <string>

#include "../Injector/Injector.h"

std::vector<float> GetLimitsCylinderHeadTemp(std::string name)
{
	// all planes are 0 - 350
	return std::vector<float> { 0, 350 };
}

std::vector<float> PercentageConversionCylinderHeadTemp(std::vector<float> percentages, std::string name)
{
	std::vector<float> limits = GetLimitsCylinderHeadTemp(name);
	float range = limits[1] - limits[0];
	for (size_t i = 0; i < 4; i++)
	{
		float p = limits[0] + percentages[i] * range;
		percentages[i] = p;
	}
	return percentages;
}

std::vector<float> CylinderHeadTemps(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType)
{
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);
	LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);
	for (SIZE_T i = 0; i < 4; i++)
	{
		uintptr_t engineOffset = 0x190 * i;
		uintptr_t offset = 0x3dbc + (engineOffset);
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
		const size_t sizeOfData = sizeof(float);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<float*>(rawData);
	}

	return PercentageConversionCylinderHeadTemp(values, planeType);
}
