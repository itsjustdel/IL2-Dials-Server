#include <Windows.h>
#include <vector>
#include <string>

#include "../Injector/Injector.h"

std::vector<double> CylinderHeadTemps(LPVOID codeCaveAddress, HANDLE hProcessIL2)
{
	std::vector<double> values(4);
	for (SIZE_T i = 0; i < 4; i++)
	{
		//offset in cave, four addresses to read for each plane
		//first engine is + 0x280 from cave, 2nd 0x188..etc
		uintptr_t offset = 0x08 * i;
		LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x280 + offset);
		LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+0xB38);
		const SIZE_T sizeOfData = sizeof(double);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<double*>(rawData);
	}

	return values;
}