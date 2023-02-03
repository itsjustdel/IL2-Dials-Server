#include <Windows.h>
#include <vector>
#include <string>

#include "../Injector/Injector.h"

std::vector<double> CarbMixTemps(LPVOID codeCaveAddress, HANDLE hProcessIL2)
{
	std::vector<double> values(4);
	for (size_t i = 0; i < 4; i++)
	{
		//offset in cave, four addresses to read for each plane
		//first engine is + 0x240 from cave, 2nd 0x188..etc
		uintptr_t offset = 0x08 * i;
		LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x240 + offset);
		LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

		// p39 0xD70
		// p47 22 0xD30 
		// p47 28 0xD40
		// p38 0xE30,0xE38
		// p51B 0xD60
		// p51D 0xD68
		// a20 0xD88,0xD90
		// c47 0x1260, 0x1268
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+0xD70);
		const size_t sizeOfData = sizeof(double);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<double*>(rawData);
	}

	return values;
}