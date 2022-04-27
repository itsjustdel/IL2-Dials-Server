#include <windows.h>

char originalLineDynamicBody[11];//two short then a long instructions means this is just what we have to do

bool InjectionDynamicBody(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{

	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x2D);//turn needle is plus 39

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineDynamicBody, sizeof(originalLineDynamicBody), &bytesWritten);//5 is enough for jump plus address

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
	BYTE nops[6] = { 0x90, 0x90, 0x90 , 0x90, 0x90, 0x90 };
	//add a nop
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}


bool CaveDynamicBody(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x2D);//39 is where we got to with other functions
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineDynamicBody, sizeof(originalLineDynamicBody), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;


	//now we need to check if this is player plane or not - player plane memory address is stored at codeCave + 100 (it is the first thing we work out)
	//if not player plane, jump back to main code
	//to cmp we need both things we want to compare in two registers

	//push - save temp var
	BYTE push[1] = { 0x50 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), push, sizeof(push), &bytesWritten);
	totalWritten += bytesWritten;

	//this is absolute mov instruction for rax
	BYTE movToRax[2] = { 0x48, 0xA1 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), movToRax, sizeof(movToRax), &bytesWritten);
	totalWritten += bytesWritten;

	//now it will want an address to mov to rbp	 (this is where plane type address is stored)
	uintptr_t relativeAddress = (uintptr_t)toCave + 0x200 - 0x2D;// 0x39 for where this section of the cave starts
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &relativeAddress, sizeof(LPCVOID), &bytesWritten);
	totalWritten += bytesWritten;

	//compare rsi and rax
	BYTE cmp[3] = { 0x48, 0x39, 0xC6 };
	//0x08 will be relative jump to code cave plus 0x100
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), cmp, sizeof(cmp), &bytesWritten);
	totalWritten += bytesWritten;

	//pop rax back now we're done - restore temp var
	BYTE pop[1] = { 0x58 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), pop, sizeof(pop), &bytesWritten);
	totalWritten += bytesWritten;

	//if it is the player, it will be equal, else it will be not equal, so don't do anything and jump to the return addess
	BYTE jumpIfNotEqual[6] = { 0x0F, 0x85 , 0x07, 0x00, 0x00, 0x00 };	//0x07 is the one line jump in this case
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;

	//else, store rcx
	//the pointer value we want is stored in rcx, so move rcx to point in our cave for later retrieval	
	// "rcxToMem" 
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xEC, 0x01, 0x00, 0x00 }; 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of "GetPlaneType" function, and we want to jump in at +8 
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + (totalWritten - 7)));// ...still trial and error for this amount of nops?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}


bool HookDynamicBody(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionDynamicBody(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveDynamicBody(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}