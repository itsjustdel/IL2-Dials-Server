#include "Manifold.h"
#include "injector.h"
#include <Windows.h>
#include <vector>
#include <string>


std::vector<double> GermanManifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2)
{
	std::vector<double> manifoldValues(4);
	for (size_t i = 0; i < 4; i++)
	{
		//offset in cave, four addresses to read for each plane
		//first engine is + 0x180 from cave, 2nd 0x188..etc
		uintptr_t offset = 0x08 * i;
		LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x180 + offset);
		LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

		LPVOID _manifold = (LPVOID)((uintptr_t)(toStruct)+0x9F8);
		const size_t sizeOfData = sizeof(double);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, _manifold, &rawData, sizeOfData, NULL);

		manifoldValues[i] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}

std::vector<double> USManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	
	std::vector<double> manifoldValues(4);
	
	
	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x140), &targetAddress, sizeof(LPCVOID), 0);

	std::string v = "P-38J-25";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDA0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDA8), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);	
	}

	v = "A-20B";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD40), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD48), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);
	}
	
	v = "P-40E-1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD10), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-39L-1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD10), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}
		
	v = "P-47D-28";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCE0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}
	
	v = "P-51D-15";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD00), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-47D-22";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCD0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-51B-5";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCF8), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}


std::vector<double> UKManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	std::vector<double> manifoldValues(4);

	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x140), &targetAddress, sizeof(LPCVOID), 0);
		

	std::string v = "Spitfire Mk.IXe";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD30), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.Vb";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCF8), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Tempest Mk.V ser.2";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD08), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Hurricane Mk.II";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD00), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.XIV";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xE50), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Typhoon Mk.Ib";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD68), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}

std::vector<double> RUManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	std::vector<double> manifoldValues(4);

	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x140), &targetAddress, sizeof(LPCVOID), 0);


	std::string v = "Yak-7B ser.36";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xC60), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);		
	}

	v = "Yak-9 ser.1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xC60), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);		
	}

	v = "Yak-9T ser.1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xc68), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);		
	}

	//bring in line with german manifold scale
	manifoldValues[0] *= 100;

	return manifoldValues;
}