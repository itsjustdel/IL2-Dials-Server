#include "Manifold.h"
#include "../Injector/Injector.h"
#include <Windows.h>
#include <vector>
#include <string>
#include "../RUPlanes/RUPlanes.h"
#include "../GERPlanes/GERPlanes.h"
#include "../UKPlanes/UKPlanes.h"
#include "../USPlanes/USPlanes.h"

char originalLineManifold[8];

std::vector<float> GetManifoldLimits(std::string name)
{
	// RU
	if (IsYak9(name) || IsYak169(name) || IsYaks127(name) || IsYak7b36(name) ||
		IsLagg3s29(name) || IsIL2(name) || IsLa5s8(name) || IsLa5fs38(name) || IsI16(name) || IsMig3(name)
		|| IsLa5fns2(name) || IsLi2(name))
	{
		// A // B
		return std::vector<float> { 300, 1600 };
	}

	else if (IsPe2(name))
	{
		return std::vector<float> { 300, 1200 };
	}

	// GER
	if (IsBf109F4(name) || IsJu87D3(name) || IsBf109G2(name) || IsFW190A3(name) || IsBf109E7(name) ||
		IsBf109F2(name) || IsBf109G4(name) || IsFW190A5(name) || IsFW190A8(name) || IsFW190A6(name))
	{
		// A
		return std::vector<float> { 600, 1800 };
	}

	else if (IsBf109G2(name) || IsBf110E2(name) || IsBf110G2(name) || IsJu88A4(name) || IsBf109G6(name) || IsBf109G6AS(name) || IsBf109G6Late(name) || IsBf109G14(name))
	{
		// B
		return std::vector<float> { 600, 1800 };
	}

	if (IsBf109K4(name)) {
		// C
		return std::vector<float> { 600, 2000 };
	}

	if (IsFW190D9(name) || IsTa152H1(name)) {
		// D
		return std::vector<float> { 600, 2500 };
	}

	if (IsHe111H6(name) || IsJu88C6(name)) {
		// E
		return std::vector<float> { 600, 1800 };
	}

	if (IsHe111H16(name) || IsHs129B2(name) || IsME410A1(name)) {
		// F
		return std::vector<float> { 600, 1800 };
	}

	if (IsIAR80(name)) {
		return std::vector<float> { 600, 1800 }; // to check
	}

	// GER none - 
	// m2 262 A
	// Ju-52/3m g4e
	// ar 234 b2

	// ITA
	if (IsMC202s8(name)) {
		return std::vector<float> { 500, 2000 };
	}

	// UK
	if (IsTyphoonMkIb(name)) {
		return std::vector<float> { -7, 16 };
	}
	else if (IsMosquitoFBMkVIser2(name))
		return std::vector<float> { -7, 27 };

	else if (IsHurricaneMkII(name)) {
		return std::vector<float> { -7, 25 };
	}
	else if (IsSpitfireMkIXc(name)) {
		return std::vector<float> { -7, 25 };
	}
	else if (IsSpitfireMkIXe(name)) {
		return std::vector<float> { -7, 25 };
	}
	else if (IsSpitfireMkVb(name)) {
		return std::vector<float> { -7, 25 };
	}
	else if (IsSpitfireMkXIV(name) || IsSpitfireMkXIVe(name)) {
		return std::vector<float> { -7, 25 };
	}
	else if (IsTempestMkVser2(name)) {
		return std::vector<float> { -7, 25 };
	}
	else if (IsUKPlane(name))
	{
		// A // B // D
		return std::vector<float> { -9, 25 };
	}

	// US
	if (IsA20B(name)) {
		// A
		return std::vector<float> { 10, 50 };
	}
	if (IsP40E(name)) {
		// B
		return std::vector<float> { 10, 50 };
	}
	if (IsP39L(name)) {
		return std::vector<float> { 10, 75 };
		// C
	}
	if (IsP47D28(name) || IsP47D22(name)) {
		return std::vector<float> { 0, 75 };
		// D
	}
	if (IsP51D15(name) || IsP51B5(name)) {
		return std::vector<float> { 0, 100 }; //goes above dial limits
		// E
	}
	if (IsP38(name) || IsC47A(name)) {
		return std::vector<float> { 0, 75 };
		// F
	}

	return std::vector<float> { 0, 0 };
}

std::vector<float> PercentageConversionManifold(std::vector<float> percentages, std::string name)
{
	std::vector<float> limits = GetManifoldLimits(name);
	float range = limits[1] - limits[0];
	for (SIZE_T i = 0; i < 4; i++)
	{
		float p = limits[0] + percentages[i] * range;
		percentages[i] = p;
	}
	return percentages;
}

std::vector<float> Manifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType)
{
	std::vector<float> values(4);
	LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x200);

	LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);
	for (SIZE_T i = 0; i < 4; i++)
	{
		uintptr_t engineOffset = 0x190 * i;
		uintptr_t offset = 0x3da8 + 0x8 + (engineOffset);

		//all 2 engine planes have temps next to each other (so far)
		LPVOID temp = (LPVOID)((uintptr_t)(toStruct)+offset);
		const SIZE_T sizeOfData = sizeof(float);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, temp, &rawData, sizeOfData, NULL);

		values[i] = *reinterpret_cast<float*>(rawData);
	}

	return PercentageConversionManifold(values, planeType);
}

//old
std::vector<double> GermanManifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2)
{
	std::vector<double> manifoldValues(4);
	for (SIZE_T i = 0; i < 4; i++)
	{
		//offset in cave, four addresses to read for each plane
		//first engine is + 0x180 from cave, 2nd 0x188..etc
		uintptr_t offset = 0x08 * i;
		LPVOID addressToRead = (LPVOID)((uintptr_t)(codeCaveAddress)+0x280 + offset);
		LPVOID toStruct = PointerToDataStruct(hProcessIL2, addressToRead);

		LPVOID _manifold = (LPVOID)((uintptr_t)(toStruct)+0x9F8);
		const SIZE_T sizeOfData = sizeof(double);
		char rawData[sizeOfData];
		ReadProcessMemory(hProcessIL2, _manifold, &rawData, sizeOfData, NULL);

		manifoldValues[i] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}
//old
std::vector<double> USManifolds(LPVOID codeCaveAddress, HANDLE hProcess, std::string name)
{

	std::vector<double> manifoldValues(4);


	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)(codeCaveAddress)+0x240), &targetAddress, sizeof(LPCVOID), 0);

	std::string v = "P-38J-25";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDE0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDE8), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);
	}

	v = "A-20B";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD48), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD50), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-40E-1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD50), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-39L-1";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD50), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-47D-28";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD20), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-51D-15";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD40), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-47D-22";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD10), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "P-51B-5";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "C-47A";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x1220), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0x1228), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}
//old
std::vector<double> UKManifolds(LPVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	std::vector<double> manifoldValues(4);

	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave
	LPCVOID targetAddress;
	ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);


	std::string v = "Spitfire Mk.IXe";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD70), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.Vb";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Tempest Mk.V ser.2";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD48), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Hurricane Mk.II";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD40), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.XIV";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xE90), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Spitfire Mk.XIVe"; //released?
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xD38), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Typhoon Mk.Ib";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDA8), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Mosquito F.B. Mk.VI ser.2";
	if (name.compare(v) == 0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xDF8), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xE00), &rawData, sizeof(double), 0);
		manifoldValues[1] = *reinterpret_cast<double*>(rawData);
	}

	return manifoldValues;
}
//old
std::vector<double> RUManifolds(LPVOID codeCaveAddress, HANDLE hProcess, std::string name)
{
	std::vector<double> manifoldValues(4);

	//buffer
	char rawData[sizeof(double)];
	//read address saved in code cave

	if (IsMig3(name)) {
		// mig uses "german" address but a unique offset
		LPCVOID targetAddress;
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x280), &targetAddress, sizeof(LPCVOID), 0);
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xA08), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);

		//return now, number in correct format
		return manifoldValues;
	}

	std::string v = "Yak-7B ser.36";
	if (name.compare(v) == 0)
	{
		LPCVOID targetAddress;
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCA0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Yak-9 ser.1";
	if (name.compare(v) == 0)
	{
		LPCVOID targetAddress;
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCA0), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	v = "Yak-9T ser.1";
	if (name.compare(v) == 0)
	{
		LPCVOID targetAddress;
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)codeCaveAddress + 0x240), &targetAddress, sizeof(LPCVOID), 0);
		ReadProcessMemory(hProcess, (LPCVOID)((uintptr_t)targetAddress + 0xCA8), &rawData, sizeof(double), 0);
		manifoldValues[0] = *reinterpret_cast<double*>(rawData);
	}

	//bring in line with german manifold scale
	manifoldValues[0] *= 100;

	return manifoldValues;
}

bool CaveManifold(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//read address from rcx and save in codecave
	toCave = (LPVOID)((uintptr_t)(toCave)+0x6D);
	SIZE_T totalWritten = 0;
	SIZE_T bytesWritten = 0;
	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineManifold, sizeof(originalLineManifold), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;

	//first of all we need to check if this is indeed the call for the player plane
	//if r13 matches the player plane address, we are good to go
	//compare r13 and rax
	//push - save temp var
	BYTE push[1] = { 0x50 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), push, sizeof(push), &bytesWritten);
	totalWritten += bytesWritten;

	//this is absolute mov instruction for rax
	BYTE movToRax[2] = { 0x48, 0xA1 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), movToRax, sizeof(movToRax), &bytesWritten);
	totalWritten += bytesWritten;

	//now it will want an address to mov to rax	 (this is where plane type address is stored)
	uintptr_t relativeAddress = (uintptr_t)toCave + 0x200 - 0x6D;// 0x76 for where this section of the cave starts
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &relativeAddress, sizeof(LPCVOID), &bytesWritten);
	totalWritten += bytesWritten;

	//compare r12 and rax
	BYTE cmp[3] = { 0x4C, 0x39, 0xE0 };
	//0x08 will be relative jump to code cave plus 0x100
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmp, sizeof(cmp), &bytesWritten);
	totalWritten += bytesWritten;

	//pop rax back now we're done - restore temp var
	BYTE pop[1] = { 0x58 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), pop, sizeof(pop), &bytesWritten);
	totalWritten += bytesWritten;

	//if not player plane, return 
	BYTE jumpIfNotEqual[2] = { 0x75, 0x34 }; //0x34 is relative jump to return line
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	//there can be mroe than one engine, engines are indexed in rsi 
	//1DE53290007 - 48 83 FF 00
	BYTE r14EqualToZero[4] = { 0x49, 0x83, 0xFE, 0x00 };
	// if not equal, do another check for the next engine index
	BYTE r14EqualToOne[4] = { 0x49, 0x83, 0xFE, 0x01 };
	//and engine no 3?
	BYTE r14EqualToTwo[4] = { 0x49, 0x83, 0xFE, 0x02 };
	//and 4?! - I don't want to do have to figure this out again!
	BYTE r14EqualToThree[4] = { 0x49, 0x83, 0xFE, 0x03 };

	//if equal, this will set a flag to be checked on..
	//jump short is 75, next byte is relative address
	//redefine 2nd element to in jump to 0x07
	//
	jumpIfNotEqual[1] = 0x07;





	//write these	
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), r14EqualToZero, sizeof(r14EqualToZero), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xED, 0x01, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), r14EqualToOne, sizeof(r14EqualToOne), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	rcxToMem[3] = 0xE8;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), r14EqualToTwo, sizeof(r14EqualToTwo), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	rcxToMem[3] = 0xE3;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), r14EqualToThree, sizeof(r14EqualToThree), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	rcxToMem[3] = 0xDE;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of function
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 4)));// ...still trial and error for this amount of nops?original line size?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	//1D1AAB1AF67
	return 1;
}

bool InjectionManifold(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	toCave = (LPVOID)((uintptr_t)(toCave)+0x6D);
	SIZE_T bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineManifold, sizeof(originalLineManifold), &bytesWritten);

	BYTE jump = 0xE9;

	//write jump opcode
	WriteProcessMemory(hProcess, (LPVOID)src, &jump, sizeof(jump), &bytesWritten);

	//Relative address. Using 32bit data type due to close nature of jump
	uintptr_t relativeAddress = (uintptr_t)toCave - src - 5;
	LPVOID rA = (LPVOID)relativeAddress;
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01), &relativeAddress, sizeof(DWORD), &bytesWritten);

	//we need to add a nope to pad out memory so we jump back at same point we left
	BYTE nops[3] = { 0x90, 0x90, 0x90 };
	//add nops
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}

bool HookManifold(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionManifold(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveManifold(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}