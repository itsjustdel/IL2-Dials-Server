#include <windows.h>

char originalLineEngineModification[6];

bool CaveEngineModification(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	SIZE_T totalWritten = 0;
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0xBF);//C8 is where we got to with other functions
	//cave - where we put our own code alongside the original
	SIZE_T bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineEngineModification, sizeof(originalLineEngineModification), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;


	//the pointer value we want is stored in rcx, so move rcx to point in our cave for later retrieval

	// "rbxToMem" - relative jump
	//48 89 15 DB 00 00 00
	BYTE rbxToMem[7] = { 0x48, 0x89, 0x15, 0xDB, 0x01, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rbxToMem, sizeof(rbxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of function
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 2)));// ...still trial and error for this amount of nops?original line size?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}

bool InjectionEngineModification(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	toCave = (LPVOID)((uintptr_t)(toCave)+0xBF);
	SIZE_T bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineEngineModification, sizeof(originalLineEngineModification), &bytesWritten);

	BYTE jump = 0xE9;

	//write jump opcode
	WriteProcessMemory(hProcess, (LPVOID)src, &jump, sizeof(jump), &bytesWritten);

	//Relative address. Using 32bit data type due to close nature of jump
	uintptr_t relativeAddress = (uintptr_t)toCave - src - 5;
	LPVOID rA = (LPVOID)relativeAddress;
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01), &relativeAddress, sizeof(DWORD), &bytesWritten);

	//we need to add a nope to pad out memory so we jump back at same point we left
	BYTE nops[1] = { 0x90 };

	//add nops
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}

bool HookEngineModification(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionEngineModification(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveEngineModification(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}