#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <sstream>   

#include "Main.h";

//original instructions in function will be put here - how long are these? Do they change on updates?
//could need to write a parser in future
char originalLineCockpit[8];//7 for mov inst, 1 for ret
char originalLineAltimeter[8];
char originalLinePlaneType[8];

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
	//char originalLine[8];//7 for mov inst, 1 for ret
	//read line
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineCockpit, 0x08, &bytesWritten);

	//check if code at this point isn't our own- happens if server restarted but game kept running
	//look for a jump code(the one we will write below if this fails)
	char charJump = L'é';//jmp code in char - if we want to make this neater, read to LPVOIDS instead of char[] and check it writes correctly
	if (originalLineCockpit[0] == charJump)
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
		codeCaveAddress = AllocateMemoryUp(hProcess, (uintptr_t)moduleRSE.modBaseAddr);

		if (codeCaveAddress == 0)
			//fail
			return 0;
		

	}

	return 1;

}

bool Injection(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//overwrite cockpit function
	//move the "mov" instruction down so we can read rbx and rax before it changes it
	

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
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01), &relativeAddress, sizeof(DWORD), &bytesWritten);//note sizeof(dword) to force 32bit

	BYTE nops[2] = { 0x90, 0x90 };
	//add a nop
	WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);//note sizeof(dword) to force 32bit
	

	return 1;
}

bool InjectionAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x2F);//0x38 is after other code stops

	size_t bytesWritten = 0;
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

bool InjectionPlaneType(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{

	//src is beginning of function but we will inject at the end
	
	src += 0x454;
	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x45);//0x38 is after the other code in the cave stops

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLinePlaneType, 0x08, &bytesWritten);

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

bool CaveCockpitInstruments(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//cave - where we put our own code 
	size_t bytesWritten = 0;
	//NOTE: If using the aob scan with the qword ptr jump, perhaps look up "symbols" and find path to function being called FF 15 4A60B500 (first two bytes are call/jmp opcodes,last part is a variable/symbol lookup?)
	
	/*
	* 
	asm:
	//if else statement,
	if not zero, copy rbx to memeroy location
	code:
     test rbx, 0
     jne RBX_NOT_ZERO
     jmp return

	RBX_NOT_ZERO:
		//if rbx NOT zero, compare with rax
		test rax, rbx
		je RBX_AND_RAX_EQUAL
		jmp return

	RBX_AND_RAX_EQUAL:
		mov [newmem+30],rbx
		jmp return

	RET_EQUAL:
    mov [newmem+20],rbx
    

	2782BC10000 - 48 F7 C3 00000000     - test rbx,00000000 { 0 }
	2782BC10007 - 0F85 05000000         - jne 2782BC10012
	2782BC1000D - E9 B461E202           - jmp RSE.RSE::CAeroplane::getPointerToCockpitInstruments+6
	2782BC10012 - 48 85 D8              - test rax,rbx
	2782BC10015 - 0F84 05000000         - je 2782BC10020
	2782BC1001B - E9 A661E202           - jmp RSE.RSE::CAeroplane::getPointerToCockpitInstruments+6
	2782BC10020 - 48 89 1D 09000000     - mov [2782BC10030],rbx { (0) }
	2782BC10027 - E9 9A61E202           - jmp RSE.RSE::CAeroplane::getPointerToCockpitInstruments+6
     
	 48 F7 C3 00 00 00 00 
	 0F 85 05 00 00 00 
	 E9 B4 61 E2 02 
	 48 85 D8 
	 0F 84 05 00 00 00 
	 E9 A6 61 E2 02 
	 48 89 1D 09 00 00 00 
	 E9 9A 61 E2 02
	*/

	BYTE ifEqualRbxToMem[35] = { 0x48, 0x85, 0xDB, //test rbx, rbx) - is zero)
								 0x0F, 0x85, 0x05, 0x00, 0x00, 0x00, //jump if not equal
								 0xE9, //jmp to..								 
								 0x15, 0x00,0x00,0x00,//26 byte (+1) jump to end of this instruction block (note 0x21 is hex) (relative address)
								 0x48, 0x39, 0xD8, //compare rax with rbx
								 0x0F, 0x84, 0x05, 0x00, 0x00, 0x00,// jump if equal (je is 0x84) / if not equal we jump to end
								 0xE9, //jmp to..
								 0x07, 0x00, 0x00, 0x00,//2 line jump -1
								 0x48, 0x89, 0x1D, 0x3D, 0x00, 0x00, 0x00 };// ,  //mov rbx to [memory] (48 89 1D is move rbx to.. the last four is the relative jump
																										
								 
	//write above array
	WriteProcessMemory(hProcess, toCave, ifEqualRbxToMem, sizeof(ifEqualRbxToMem), &bytesWritten);

	//and write orignal back in after our code
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+sizeof(ifEqualRbxToMem)), &originalLineCockpit, 7, &bytesWritten);//7 is without ret

	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+sizeof(ifEqualRbxToMem) + 7) , &jump, sizeof(jump), &bytesWritten);

	DWORD returnAddress = (uintptr_t)(src - (uintptr_t)toCave - sizeof(ifEqualRbxToMem) - 5);//jump call plus ... initial overwrite?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+sizeof(ifEqualRbxToMem) + 7 + 1), &returnAddress, sizeof(returnAddress), &bytesWritten);

	//now we need to write a signature in so we can check if code cave exists - Happens on server restart but game stays active

	//DellyWellyCockpit in Hex
	//BYTE mySig[17] = { 0x44, 0x65, 0x6c, 0x6c, 0x79, 0x57, 0x65, 0x6c, 0x6c, 0x79, 0x43, 0x6f, 0x63, 0x6b, 0x70, 0x69, 0x74 };
	//WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 0x40), mySig, 17, &bytesWritten);

	return 1;
}

bool CaveAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave =(LPVOID)( (uintptr_t)(toCave) + 0x2F);//0x38 is after other code stops
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;
	
	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineAltimeter, 7, &bytesWritten);//7 is without ret

	//the pointer value we want is stored in rax, so move rax to point in our cave for later retrieval
	BYTE raxToMem[2] = { 0x48, 0xA3 };	
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 7 ), raxToMem, sizeof(raxToMem), &bytesWritten);

	LPVOID targetAddress = (LPVOID)((uintptr_t)(toCave)-0x2f +0x80);
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 7 + sizeof(raxToMem)), &targetAddress, sizeof(LPVOID), &bytesWritten);	

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+ 7 + sizeof(raxToMem) + sizeof(LPVOID)), &jump, sizeof(jump), &bytesWritten);//HERE JUMP GOIONG IN WRONG PLACE?

	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + 7 + 2 + 4 +1 + 1));//jump call plus ... initial overwrite? (plus padding)
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 7 + sizeof(raxToMem) + sizeof(LPVOID) + sizeof(jump)), &returnAddress, sizeof(returnAddress), &bytesWritten);

	
	

	//now we need to write a signature in so we can check if code cave exists - Happens on server restart but game stays active
	//"DellyWellyAltimeter" in Hex = 44656c6c7957656c6c79416c74696d65746572 + 0x00 to pad it so next line starts on a new line of memory
	//BYTE mySig[20] = { 0x44, 0x65, 0x6c, 0x6c, 0x79, 0x57, 0x65, 0x6c, 0x6c, 0x79, 0x41, 0x6c, 0x74, 0x69, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x00 }; //PUT AFTYER JUMP BACK
	//WriteProcessMemory(hProcess, whereWeGotTo, mySig, 19, &bytesWritten);

	return 1;
}

bool CavePlaneType(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x45);//0x?? is after other code stops
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLinePlaneType, 7, &bytesWritten);//7 is without ret

	//the pointer value we want is stored in r11, so move r11 to point in our cave for later retrieval
	//bytes ex. 4C 89 1D 29 00 00 00
	BYTE r11ToMem[3] = { 0x4C, 0x89, 0x1D };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+7), r11ToMem, sizeof(r11ToMem), &bytesWritten);

	//relative address from inst, but we want to be "0xAO" after code cave start
	//if we look at the debugger we know we are at line 0x45
	// - 0xA0 - 0x4C is our relative jump
	LPVOID targetAddress = (LPVOID)(0xA0 - 0x4C - 0x07); 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+7 + sizeof(r11ToMem)), &targetAddress, sizeof(LPVOID), &bytesWritten);

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+7 + sizeof(r11ToMem) + sizeof(LPVOID)), &jump, sizeof(jump), &bytesWritten);//HERE JUMP GOIONG IN WRONG PLACE?

	//0x17 takes us back to start of "GetPlaneType" function and 0x45B takes us where we need ot jump back to
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave +( 0x17 - 0x45B))); 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+7 + sizeof(r11ToMem) + sizeof(LPVOID) + sizeof(jump)), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}





bool HookCockpitInstruments(HANDLE hProcess, void* pSrc, size_t size,LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;	

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;


	//insert jump in to original code
	if (!Injection(hProcess, src, codeCaveAddress))
		return 0;

	//write out own process in our own allocated memory
	if (!CaveCockpitInstruments(hProcess, src, codeCaveAddress))
		return 0;

	
	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
	return 1;
}

bool HookAltimeter(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
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
	//DWORD dwOld;
	//if (!VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, PAGE_EXECUTE_READWRITE, &dwOld))
		//return 0;

	//read process memory returns to this
	LPVOID pointer= 0;
	
	size_t bytesRead = 0;
	//attempt to read from address passed by Inject function	
	ReadProcessMemory(hProcess, readAddress, &pointer, sizeof(LPCVOID), &bytesRead);

	////put write protections back to what they were before we injected
	//VirtualProtectEx(hProcess, (LPVOID)AOBresult, size, dwOld, &dwOld);


	return pointer;
}
