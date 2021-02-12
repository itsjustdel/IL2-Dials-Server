#pragma once
#include <Windows.h>
#include <TlHelp32.h>
bool Injection(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//for debug
	size_t bytesWritten = 0;

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

bool InjectionAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//for debug
	size_t bytesWritten = 0;

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
	BYTE nop = 0x90;
	//note we add the size of our relative address to the write address so we keep moving forward when writing
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nop, sizeof(nop), &bytesWritten);



	return 1;
}

bool CaveCockpitInstruments(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;
	//NOTE: If using the aob scan with the qword ptr jump, perhaps look up "symbols" and find path to function being called FF 15 4A60B500 (first two bytes are call/jmp opcodes,last part is a variable/symbol lookup?)
	//Using what seems to be static additions for aob for now (2nd Aob scan in CE)

	//copy rbx to our memory location - rbs holds the pointer address to the cockpit struct
	//these are the opcodes for that when doping it with cheat engine
	//7FF6AFB10000 - mov [7FF6AFB10018],rbx =  48 89 1D 11000000    (relative)
	//so, 0x48, 0x89, 0x1d, 0x11, 0x00, 0x00, 0x00

	//enter opcodes that we ripped out to do the jump
	//original function = 0x48, 0x89, 0x6C, 0x24, 0x68

	//join together (put all writes together?)
	BYTE partialFunction[] = { 0x48, 0x89, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x48, 0x89, 0x6C, 0x24, 0x68 };
	WriteProcessMemory(hProcess, toCave, partialFunction, sizeof(partialFunction), &bytesWritten);

	//now jump it back to the end of the original function
	//where we got to, writing bytes
	//casting to uintptr so we can do maths
	LPVOID whereWeGotTo = (LPVOID)((uintptr_t)toCave + sizeof(partialFunction));

	//0x09 is the byte form of "jmp", assembly language to jump to a location. Note this is a x86 instruction (it can only jump +- 2gb of memory)
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, whereWeGotTo, &jump, sizeof(jump), &bytesWritten);
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + 0x1);

	//write address to jump back to	(just starting at one after jmp instruction) jmp 32 bit
	//truncate 64bit to 32 bit - could write a check for non zero in upper register
	DWORD returnAddress = (uintptr_t)(src - (uintptr_t)toCave) - 5 - 7;//jump call plus move rbx to mem	
	WriteProcessMemory(hProcess, whereWeGotTo, &returnAddress, sizeof(returnAddress), &bytesWritten);
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + sizeof(DWORD));
	//leave space in memory for data struct pointer value to be written, add enough on so signature starts nicely at next block
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + 15);


	//now we need to write a signature in so we can check if code cave exists - Happens on server restart but game stays active
	//"DellyWellyAltimeter" in Hex = 44656c6c7957656c6c79416c74696d65746572 + 0x00 to pad it so next line starts on a new line of memory
	BYTE mySig[20] = { 0x44, 0x65, 0x6c, 0x6c, 0x79, 0x57, 0x65, 0x6c, 0x6c, 0x79, 0x41, 0x6c, 0x74, 0x69, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x00 }; //PUT AFTYER JUMP BACK
	WriteProcessMemory(hProcess, whereWeGotTo, mySig, 19, &bytesWritten);

	return 1;
}

bool CaveAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//new cave in RSE dll
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;
	

	
	//track where we got o
	LPVOID whereWeGotTo = (LPVOID)((uintptr_t)toCave);
	//opcodes to write rbx to our memory at 0x18 offset
	//24A3B6D0000 - 48 89 1D 11000000 - mov[24A3B6D0018], rbx{ (0) }

	//afterwards enter opcodes that we ripped out to do the jump
	//original function = F2 0F10 44 24 60

	//join together with opcodes that move register value to our memory
	BYTE partialFunction[] = { 0x48, 0x89, 0x1d, 0x11, 0x00, 0x00, 0x00, 0xF2, 0x0F, 0x10, 0X44, 0x24, 0x60 };

	WriteProcessMemory(hProcess, whereWeGotTo, partialFunction, sizeof(partialFunction), &bytesWritten);

	//now jump it back to the end of the original function
	//where we got to, writing bytes
	//casting to uintptr so we can do maths
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + sizeof(partialFunction));

	//0x09 is the byte form of "jmp", assembly language to jump to a location. Note this is a x86 instruction (it can only jump +- 2gb of memory)
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, whereWeGotTo, &jump, sizeof(jump), &bytesWritten);
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + 0x1);

	//write address to jump back to	(just starting at one after jmp instruction) jmp 32 bit
	DWORD returnAddress = (uintptr_t)(src - (uintptr_t)toCave) - 5 - 7 ;//jump call plus move rbx to mem -

	//truncate 64bit to 32 bit
	WriteProcessMemory(hProcess, whereWeGotTo, &returnAddress, sizeof(returnAddress), &bytesWritten);
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + sizeof(DWORD));
	//leave space in memory for data struct pointer value to be written, add enough on so signature starts nicely at next block
	whereWeGotTo = (LPVOID)((uintptr_t)whereWeGotTo + 14);
	//now we need to write a signature in so we can check if code cave exists - Happens on server restart but game stays active
	//"DellyWellyAltimeter" in Hex = 44656c6c7957656c6c79416c74696d65746572 + 0x00 to pad it so next line starts on a new line of memory
	BYTE mySig[20] = { 0x44, 0x65, 0x6c, 0x6c, 0x79, 0x57, 0x65, 0x6c, 0x6c, 0x79, 0x41, 0x6c, 0x74, 0x69, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x00 }; //PUT AFTYER JUMP BACK
	WriteProcessMemory(hProcess, whereWeGotTo, mySig, 19, &bytesWritten);

	return 1;
}

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



bool HookCockpitInstruments(HANDLE hProcess, void* pSrc, size_t size, LPVOID toCave)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;	

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	Injection(hProcess, src, toCave);

	//write out own process in our own allocated memory
	CaveCockpitInstruments(hProcess, src, toCave);
	
	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}

bool HookAltimeter(HANDLE hProcess, void* pSrc, size_t size, LPVOID toCave)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionAltimeter(hProcess, src, toCave);

	//write out own process in our own allocated memory - 
	CaveAltimeter(hProcess, src, toCave);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
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

LPVOID PointerToDataStruct(HANDLE hProcess, LPVOID caveStart)
{
	//read only possible after injection? allocate mem at read only?
	//DWORD dwOld;
	//if (!VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, PAGE_EXECUTE_READWRITE, &dwOld))
		//return 0;

	//read process memory returns to this
	LPVOID pointerToCockpitInstruments = 0;
	//uintptr_t lets us do pointer arithmetic
	uintptr_t readAddress = (uintptr_t)caveStart + 0x18; //18 is our offset set by ourselves

	size_t bytesRead = 0;
	//attempt to read from address passed by Inject function	
	ReadProcessMemory(hProcess, (LPCVOID)readAddress, &pointerToCockpitInstruments, sizeof(LPCVOID), &bytesRead);

	////put write protections back to what they were before we injected
	//VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, dwOld, &dwOld);


	return pointerToCockpitInstruments;
}
