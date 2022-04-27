#include <wtypes.h>
#include "PlaneType.h"

char originalLinePlaneType[8];


bool InjectionPlaneType(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{

	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x00);//plane type is a tstart of code cave

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLinePlaneType, 8, &bytesWritten);//5 is enough for jump plus address

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
	BYTE nops[3] = { 0x90, 0x90, 0x90 };
	//add a nop
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}

bool CavePlaneType(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x00);//0x00 - plane type at start of cave
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLinePlaneType, 8, &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;

	//only read address from rcx if rax == 0
	BYTE cmp[4] = { 0x48, 0x83, 0xF8, 0x00 };///???? player is 01, all is 00
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmp, sizeof(cmp), &bytesWritten);
	totalWritten += bytesWritten;

	//jump short if equal - 
	BYTE je[2] = { 0x74, 0x07 };///???? player is 01, all is 00
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), je, sizeof(je), &bytesWritten);
	totalWritten += bytesWritten;
	//copy rcx to code cave plus 200 - Use cheat engine to view the assembly
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xEB, 0x01, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of "GetPlaneType" function, and we want to jump in at +8 
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 4)));//+4, ...still trial and error for this
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}

bool HookPlaneType(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionPlaneType(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CavePlaneType(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}