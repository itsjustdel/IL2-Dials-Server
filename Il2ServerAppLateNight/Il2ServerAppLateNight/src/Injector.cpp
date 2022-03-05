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
char originalLineTurnNeedle[11];//two short then a long instructions means this is just what we have to do
char originalLineTurnBall[5]; //perfect - only 5 bytes needed for jump and original instruction is 5 bytes
char originalLineManifold[8];

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
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLinePlaneType, 0x08, &bytesWritten);

	//check if code at this point isn't our own- happens if server restarted but game kept running
	//look for a jump code(the one we will write below if this fails)
	char charJump = L'é';//jmp code in char - if we want to make this neater, read to LPVOIDS instead of char[] and check it writes correctly
	if (originalLinePlaneType[0] == charJump)
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
	toCave = (LPVOID)((uintptr_t)(toCave)+0x23);//0x23 is after other code stops

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

	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x00);//plane type is a tstart of code cave

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLinePlaneType,8, &bytesWritten);//5 is enough for jump plus address

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


bool InjectionTurnNeedle(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{

	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x39);//turn needle is plus 39

	size_t bytesWritten = 0;
	ReadProcessMemory(hProcess, (LPVOID)src, &originalLineTurnNeedle, sizeof(originalLineTurnNeedle), &bytesWritten);//5 is enough for jump plus address

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


bool InjectionTurnBall(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{

	//note, replicated in cave code
	toCave = (LPVOID)((uintptr_t)(toCave)+0x65);//turn ball is plus ??

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

	//we don't need to pad anything, this instruction fits exactly over the previous line

	//we need to add a nope to pad out memory so we jump back at same point we left
//	BYTE nops[6] = { 0x90, 0x90, 0x90 , 0x90, 0x90, 0x90 };
	//add a nop
	//WriteProcessMemory(hProcess, (LPVOID)(src + 0x01 + sizeof(DWORD)), &nops, sizeof(nops), &bytesWritten);

	return 1;
}

bool InjectionManifold(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	toCave = (LPVOID)((uintptr_t)(toCave)+0x76);
	size_t bytesWritten = 0;
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
								 0x48, 0x89, 0x1D, (0x120 -0x23), 0x00, 0x00, 0x00 };// ,  //mov rbx to [memory] (48 89 1D is move rbx to.. the last four is the relative jump 
																										
								 
	//write above array
	WriteProcessMemory(hProcess, toCave, ifEqualRbxToMem, sizeof(ifEqualRbxToMem), &bytesWritten);

	//and write orignal back in after our code
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+sizeof(ifEqualRbxToMem)), &originalLineCockpit, 7, &bytesWritten);//7 is without ret

	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+sizeof(ifEqualRbxToMem) + 7) , &jump, sizeof(jump), &bytesWritten);

	DWORD returnAddress = (uintptr_t)(src - (uintptr_t)toCave - sizeof(ifEqualRbxToMem) - 5);//jump call plus ... initial overwrite?
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+sizeof(ifEqualRbxToMem) + 7 + 1), &returnAddress, sizeof(returnAddress), &bytesWritten);
		
	return 1;
}

bool CaveAltimeter(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave =(LPVOID)( (uintptr_t)(toCave) + 0x23);//0x23 is after other code stops
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;
	
	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineAltimeter, 7, &bytesWritten);//7 is without ret

	//the pointer value we want is stored in rax, so move rax to point in our cave for later retrieval
	BYTE raxToMem[2] = { 0x48, 0xA3 };	
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 7 ), raxToMem, sizeof(raxToMem), &bytesWritten);

	LPVOID targetAddress = (LPVOID)((uintptr_t)(toCave)-0x23 +0x120);
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 7 + sizeof(raxToMem)), &targetAddress, sizeof(LPVOID), &bytesWritten);	

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+ 7 + sizeof(raxToMem) + sizeof(LPVOID)), &jump, sizeof(jump), &bytesWritten);//HERE JUMP GOIONG IN WRONG PLACE?

	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave + 7 + 2 + 4 +1 + 1));//jump call plus ... initial overwrite? (plus padding)
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) + 7 + sizeof(raxToMem) + sizeof(LPVOID) + sizeof(jump)), &returnAddress, sizeof(returnAddress), &bytesWritten);

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



	//Bytes to check if rax is 0 or 1 and then push rcx 
	//cmp rax,00
	//48 83 F8 00 
	//jne 5 bytes
	//0F 85 05 00 00 00 
	//rcx to .. a nice place
	//E9 E2 59 E7 02 48 89 0D 25 00 00 00 //(25 00 00 00) being the relative address in the example
	//jmp back to original code
	//E9 D6 59 E7 02

	//the pointer value we want is stored in rcx, so move rcx to point in our cave for later retrieval
	//bytes ex. 4C 89 1D 29 00 00 00
	BYTE cmp[4] = { 0x48, 0x83, 0xF8, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+ totalWritten), cmp, sizeof(cmp), &bytesWritten);
	totalWritten += bytesWritten;

	//jump near if equal - 
	BYTE jne[6] = { 0x0F, 0x85, 0x05, 0x00, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jne, sizeof(jne), &bytesWritten);
	totalWritten += bytesWritten;
	//first part of "rcxToMem" is the simple small jump to skip the rcx to mem instruction	
	BYTE rcxToMem[12] = { 0xE9,  0x07, 0x00, 0x00, 0x00, 0x48, 0x89, 0x0D, (0x100 - 0x1E), 0x00, 0x00, 0x00 }; //sum in brackets to get relative address (we are at 1E)
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	//jump to return address
	BYTE jump = 0xE9;
	//write 0x09 (jmp) 
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), &jump, sizeof(jump), &bytesWritten);
	totalWritten += bytesWritten;
	//bytes written takes us back to start of "GetPlaneType" function, and we want to jump in at +8 
	DWORD returnAddress = (uintptr_t)(src - ((uintptr_t)toCave +(totalWritten -4) ));//+4, ...still trial and error for this
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave) +totalWritten), &returnAddress, sizeof(returnAddress), &bytesWritten);

	return 1;
}

bool CaveTurnNeedle(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x39);//39 is where we got to with other functions
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineTurnNeedle, sizeof(originalLineTurnNeedle), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;


	//now we need to check if this is player plane or not - player plane memory address is stored at codeCave + 100 (it is the first thing we work out)
	//if not player plane, jump back to main code
	//to cmp we need both things we want to compare in two registers

	//push - save temp var
	BYTE push[1] = { 0x50 };//pop rax on to stack
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), push, sizeof(push), &bytesWritten);
	totalWritten += bytesWritten;

	//this is absolute mov instruction for rax
	BYTE movToRax[2] = { 0x48, 0xA1};
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), movToRax, sizeof(movToRax), &bytesWritten);
	totalWritten += bytesWritten;

	//now it will want an address to mov to rbp	 (thi si shwre plane type address is tored)
	uintptr_t relativeAddress = (uintptr_t)toCave + 0x100 - 0x39;// 0x39 for where this section of the cave starts
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
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, (0x140 - 0x60), 0x00, 0x00, 0x00 }; //sum in brackets to get relative address (next line is 60) and +0x140 is where var will be
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

bool CaveTurnBall(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	size_t totalWritten = 0;
	//cave in RSE dll already has some cockpit instruments stuff in it so we will put our code after it
	//add to cave,( uintptr_t for addition)
	toCave = (LPVOID)((uintptr_t)(toCave)+0x65);//63 is where we got to with other functions
	//cave - where we put our own code alongside the original
	size_t bytesWritten = 0;

	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineTurnBall, sizeof(originalLineTurnBall), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;


	//the pointer value we want is stored in rcx, so move rcx to point in our cave for later retrieval

	// "rcxToMem" //sum in brackets to get relative address (we are at xx) and +0x160 is where var will be (relative jump calcualted from next line)
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xEF, 0x00, 0x00, 0x00 }; //EF new jump - keeping this info... //0x104 is the jump, spread over two bytes ( bytes reversed - little endian /big)
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


bool CaveManifold(HANDLE hProcess, uintptr_t src, LPVOID toCave)
{
	//read address from rcx and save in codecave
	toCave = (LPVOID)((uintptr_t)(toCave)+0x76);
	size_t totalWritten = 0;
	size_t bytesWritten = 0;
	//first of all write the original function back in
	//and write orignal back in after our code
	WriteProcessMemory(hProcess, toCave, &originalLineManifold, sizeof(originalLineManifold), &bytesWritten);//5 is enough for the jump plus address
	totalWritten += bytesWritten;

	

	//there can be mroe than one engine, engines are indexed in rsi 
	//1DE53290007 - 48 83 FF 00
	BYTE rsiEqualToZero[4] = { 0x48, 0x83, 0xFE, 0x00 };	
	 // if not equal, do another check for the next engine index
	BYTE rsiEqualToOne[4] = { 0x48, 0x83, 0xFE, 0x01 };	
	//and engine no 3?
	BYTE rsiEqualToTwo[4] = { 0x48, 0x83, 0xFE, 0x02 };	
	//and 4?! - I don't want to do have to figure this out again!
	BYTE rsiEqualToThree[4] = { 0x48, 0x83, 0xFE, 0x03 };

	//if equal, this will set a flag to be checked on..
	//jump short is 74, next byte is relative address
	BYTE jumpIfNotEqual[2] = { 0x75, 0x07}; 

	
	
	

	//write these	
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToZero, sizeof(rsiEqualToZero), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;
	BYTE rcxToMem[7] = { 0x48, 0x89, 0x0D, 0xF5, 0x00, 0x00, 0x00 };
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToOne, sizeof(rsiEqualToOne), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;
	//need to adjust target address
	rcxToMem[3] = 0xF0;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToTwo, sizeof(rsiEqualToTwo), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;
	//need to adjust target address
	rcxToMem[3] = 0xEB;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rcxToMem, sizeof(rcxToMem), &bytesWritten);
	totalWritten += bytesWritten;

	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), rsiEqualToThree, sizeof(rsiEqualToThree), &bytesWritten);
	totalWritten += bytesWritten;
	WriteProcessMemory(hProcess, (LPVOID)((uintptr_t)(toCave)+totalWritten), jumpIfNotEqual, sizeof(jumpIfNotEqual), &bytesWritten);
	totalWritten += bytesWritten;
	//need to adjust target address
	rcxToMem[3] = 0xE6;
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

bool HookTurnNeedle(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
{
	//save old read/write access to put back to how it was later
	DWORD dwOld;

	if (!VirtualProtectEx(hProcess, pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	uintptr_t src = (uintptr_t)pSrc;
	//insert jump in to original code
	InjectionTurnNeedle(hProcess, src, codeCaveAddress);

	//write out own process in our own allocated memory - 
	CaveTurnNeedle(hProcess, src, codeCaveAddress);

	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcess, pSrc, size, dwOld, &dwOld);

	//return the start of our allocated memory struct
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


bool HookManifold(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress)
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
