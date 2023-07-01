#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../Injector/Injector.h"

std::vector<float> Fuel(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeName) {
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);

	LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);
	for (size_t i = 0; i < 4; i++)
	{
		uintptr_t engineOffset = 0x04 * i;		
		uintptr_t offset = 0x3428 + (engineOffset);

		//all 2 engine planes have temps next to each other (so far)
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
		const size_t sizeOfData = sizeof(float);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<float*>(rawData);
	}

	return values;
}