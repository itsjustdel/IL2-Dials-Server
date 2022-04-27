#include <windows.h>
char originalLineTurnBall[5]; //perfect - only 5 bytes needed for jump and original instruction is 5 bytes

bool CaveTurnBall(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x59);//65 is where we got to with other functions
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineTurnBall, sizeof(originalLineTurnBall), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;

	//the pointer value we want is stored in rcx, so move rcx to point in our cave for later retrieval	
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xFB, 0x01, 0x00, 0x00 }; 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of function
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 1)));// ...still trial and error for this amount of nops?original line size?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}

bool InjectionTurnBall(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{

	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x59);//turn ball is plus ??

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineTurnBall, sizeof(originalLineTurnBall), &bytesWritten);//5 is enough for jump plus address

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

	return 1;
}

bool HookTurnBall(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionTurnBall(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveTurnBall(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}