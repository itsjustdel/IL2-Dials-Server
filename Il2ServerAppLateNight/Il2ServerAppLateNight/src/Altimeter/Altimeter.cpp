#include <windows.h>

char originalLineAltimeter[7];

bool InjectionAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x1A);//0x23 is after other code stops

	SIZE_T bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineAltimeter, 0x08, &bytesWritten);

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
	//we need to add a nope to pad out memory so we jump back at same point we left
	BYTE nops[2] = { 0x90, 0x90 };
	//add a nop
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);



	return 1;
}

bool CaveAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x1A);
	//cave - where we put our own code alongside the original

	SIZE_T totalWritten = 0;
	SIZE_T bytesWritten = 0;

	WriteProcessMemory(hProcess, toCave, &originalLineAltimeter, 7, &bytesWritten);//7 is without ret
	totalWritten += bytesWritten;

	//the pointer value we want is stored in rax, so move rax to point in our cave for later retrieval
	BYTE raxToMem[2] = { 0x48, 0xA3 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), raxToMem, sizeof(raxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//note rax doesn't have a relative mov instrution 
	LPVOID targetAddress = (LPVOID)((uintptr_t)(toCave)-0x1A + 0x220);
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &targetAddress, sizeof(LPVOID), &bytesWritten);;
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);//HERE JUMP GOIONG IN WRONG PLACE?
	totalWritten += bytesWritten;

	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + totalWritten - 3));
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}

bool HookAltimeter(HANDLE hProcess, void* pSrc, SIZE_T size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionAltimeter(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveAltimeter(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}
