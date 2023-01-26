#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <sstream>
#include "../../src/Main/Main.h";


LPVOID AllocateMemory(HANDLE hProcess, uintptr_t src)
{
	//to return, the address where we found the memory
	LPVOID toCave = 0;

	//find unallocated memory
	MEMORY_BASIC_INFORMATION mbi;

	for (SIZE_T addr = (SIZE_T)src; addr > (SIZE_T)src - 0x80000000; addr = (SIZE_T)mbi.BaseAddress - 1)
	{
		if (!VirtualQueryEx(hProcess, (LPCVOID)addr, &mbi, sizeof(mbi)))
		{
			break;
		}
		//scan through until FREE block is discovered
		if (mbi.State == MEM_FREE)
		{
			if (toCave = VirtualAllocEx(hProcess, mbi.BaseAddress, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))//64kb size, smallest granulation/and write privelages need to be kept open
			{

				return toCave;// break;
			}
		}
	}

	return 0;
}

LPVOID AllocateMemoryUp(HANDLE hProcess, uintptr_t src)
{
	//to return, the address where we found the memory
	LPVOID toCave = 0;

	//find unallocated memory
	MEMORY_BASIC_INFORMATION mbi;

	size_t size = 0x1000;

	for (SIZE_T addr = src; addr < src + 2147483648; addr += size)
		//for (SIZE_T addr = (SIZE_T)src; addr > (SIZE_T)src - 0x80000000; addr = (SIZE_T)mbi.BaseAddress - 1)
	{
		if (!VirtualQueryEx(hProcess, (LPCVOID)addr, &mbi, sizeof(mbi)))
		{
			break;
		}
		//scan through until FREE block is discovered
		if (mbi.State == MEM_FREE)
		{
			if (toCave = VirtualAllocEx(hProcess, mbi.BaseAddress, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
			{

				return toCave;// break;
			}
		}
	}

	return 0;
}

bool CodeCave(HANDLE hProcess, uintptr_t src, MODULEENTRY32 moduleRSE, LPVOID& codeCaveAddress)
{
	size_t bytesWritten = 0;
	char originalLine[8];//7 for mov inst, 1 for ret
	//read line
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLine, 0x08, &bytesWritten);

	//check if code at this point isn't our own- happens if server restarted but game kept running
	//look for a jump code(the one we will write below if this fails)
	char charJump = L'é';//jmp code in char - if we want to make this neater, read to LPVOIDS instead of char[] and check it writes correctly
	if (originalLine[0] == charJump)
	{
		//This is the jump instruction, the one we already put in
		//we need to find where its jumping to
		//reading in to LPVOID mirrors the representation of bytes in the memory debugger
		LPVOID address = NULL;
		ReadProcessMemory(hProcess, (LPVOID)(src + 0x01), &address, 0x04, &bytesWritten);

		//add the starting address of getPointerToCockpitInstruments to the address that was stored previously plus the length to the next instruction
		codeCaveAddress = LPVOID(src + (uintptr_t)(address) + 0x05);

		CaveRecovered();
		
		return 1;
	}
	else
	{
		//create cave
		//codeCaveAddress = AllocateMemoryUp(hProcess, (uintptr_t)moduleRSE.modBaseAddr);
		codeCaveAddress = AllocateMemoryUp(hProcess, (uintptr_t)moduleRSE.modBaseAddr);

		if (codeCaveAddress == 0)
			//fail
			return 0;
		
	}

	return 1;

}

bool RestoreOriginalMemory(HANDLE hProcess, LPVOID AOBresult,LPVOID caveStart,SIZE_T size, char* originalMemory)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;
	if (!VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	//release code cave memory back to the wild
	VirtualFreeEx(hProcess, caveStart, 0, MEM_RELEASE);

	size_t bytesWritten = 0;

	//put back - array pointers decay to first element in array so need to copy data this way to a local variable
	char test[100];//how to pass variable for this??
	for (int i = 0; i < 100; ++i)
		test[i] = originalMemory[i];

	//then write it
	WriteProcessMemory(hProcess, (LPVOID)AOBresult, &test,size, &bytesWritten);
	

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, dwOld, &dwOld);

	return 1;
}

bool SaveOriginalMemory(HANDLE hProcess, LPVOID AOBresult,SIZE_T size, char * outStr)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;
	if (!VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	//Save original memory so we can paste it back in after we get what we need (the address to data struct that holds what we need)
	char test[100];
	ReadProcessMemory(hProcess, (LPVOID)AOBresult, &test, size, NULL);
	for (int i = 0; i < 100; ++i) 
		outStr[i] = test[i];

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, dwOld, &dwOld);

	return 0;
}

LPVOID PointerToDataStruct(HANDLE hProcess, LPVOID readAddress)
{
	//read only possible after injection? allocate mem at read only?
//	DWORD dwOld;
	//if (!VirtualProtectEx(hProcess, (LPVOID)readAddress, 1000, PAGE_EXECUTE_READWRITE, &dwOld))
		//return 0;

	//read process memory returns to this
	LPVOID pointer= 0;
	
	size_t bytesRead = 0;
	//attempt to read from address passed by Inject function	
	ReadProcessMemory(hProcess, readAddress, &pointer, sizeof(LPCVOID), &bytesRead);

	////put write protections back to what they were before we injected
	//VirtualProtectEx(hProcess, (LPVOID)readAddress, 1000, dwOld, &dwOld);


	return pointer;
}
