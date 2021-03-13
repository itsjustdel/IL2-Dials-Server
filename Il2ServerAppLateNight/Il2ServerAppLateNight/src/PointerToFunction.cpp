#include "PointerToFunction.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <string>


LPCVOID PointerToFunction(std::string functionName, HANDLE hProcessIL2,MODULEENTRY32 moduleRSE)
{

	//save old read/write access to put back to how it was later
	DWORD dwOld;
	char* pStart = (char*)(moduleRSE.modBaseAddr);

	if (!VirtualProtectEx(hProcessIL2, pStart, moduleRSE.modBaseSize, PAGE_EXECUTE_READWRITE, &dwOld))
		return 0;

	//https://www.codeproject.com/Tips/139349/Getting-the-address-of-a-function-in-a-DLL-loaded

	//read pe header externally
	IMAGE_DOS_HEADER DosHeader{ };
	IMAGE_NT_HEADERS nt_headers{ };
	IMAGE_EXPORT_DIRECTORY exp_dir{ };
	DWORD Signature = 0;
	uintptr_t RemoteModuleBaseVA = (uintptr_t)moduleRSE.hModule;
	IMAGE_FILE_HEADER FileHeader = { 0 };
	IMAGE_OPTIONAL_HEADER64 OptHeader64 = { 0 };
	IMAGE_DATA_DIRECTORY ExportDirectory = { 0 };
	IMAGE_EXPORT_DIRECTORY ExportTable = { 0 };
	UINT_PTR ExportFunctionTableVA = 0;
	UINT_PTR ExportNameTableVA = 0;
	UINT_PTR ExportOrdinalTableVA = 0;
	DWORD* ExportFunctionTable = NULL;
	DWORD* ExportNameTable = NULL;
	WORD* ExportOrdinalTable = NULL;

	//temps
	CHAR TempChar;
	BOOL Done = FALSE;


	ReadProcessMemory(hProcessIL2, (LPCVOID)RemoteModuleBaseVA, &DosHeader, sizeof(IMAGE_DOS_HEADER), nullptr);

	if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	if (!::ReadProcessMemory(hProcessIL2, (LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew),
		&Signature, sizeof(Signature), NULL) || Signature != IMAGE_NT_SIGNATURE)
		return 0;

	if (!::ReadProcessMemory(hProcessIL2,
		(LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew + sizeof(Signature)),
		&FileHeader, sizeof(FileHeader), NULL))
		return 0;

	if (!::ReadProcessMemory(hProcessIL2,
		(LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew + sizeof(Signature) + sizeof(FileHeader)),
		&OptHeader64, FileHeader.SizeOfOptionalHeader, NULL)
		|| OptHeader64.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		return 0;

	ExportDirectory.VirtualAddress = (OptHeader64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).VirtualAddress;
	ExportDirectory.Size = (OptHeader64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).Size;

	/* Read the main export table */
	if (!::ReadProcessMemory(hProcessIL2, (LPCVOID)(RemoteModuleBaseVA + ExportDirectory.VirtualAddress),
		&ExportTable, sizeof(ExportTable), NULL))
		return 0;

	/* Save the absolute address of the tables so we don't need to keep adding the base address */
	ExportFunctionTableVA = RemoteModuleBaseVA + ExportTable.AddressOfFunctions;
	ExportNameTableVA = RemoteModuleBaseVA + ExportTable.AddressOfNames;
	ExportOrdinalTableVA = RemoteModuleBaseVA + ExportTable.AddressOfNameOrdinals;

	/* Allocate memory for our copy of the tables */
	ExportFunctionTable = new DWORD[ExportTable.NumberOfFunctions];
	ExportNameTable = new DWORD[ExportTable.NumberOfNames];
	ExportOrdinalTable = new WORD[ExportTable.NumberOfNames];

	/* Check if the allocation failed */
	if (ExportFunctionTable == NULL || ExportNameTable == NULL || ExportOrdinalTable == NULL)
		return 0;

	/* Get a copy of the function table */
	if (!::ReadProcessMemory(hProcessIL2, (LPCVOID)ExportFunctionTableVA,
		ExportFunctionTable, ExportTable.NumberOfFunctions * sizeof(DWORD), NULL))
		return 0;

	/* Get a copy of the name table */
	if (!::ReadProcessMemory(hProcessIL2, (LPCVOID)ExportNameTableVA,
		ExportNameTable, ExportTable.NumberOfNames * sizeof(DWORD), NULL))
		return 0;

	/* Get a copy of the ordinal table */
	if (!::ReadProcessMemory(hProcessIL2, (LPCVOID)ExportOrdinalTableVA,
		ExportOrdinalTable, ExportTable.NumberOfNames * sizeof(WORD), NULL))
		return 0;



	/* Iterate through all the names and see if they match the one we are looking for */
	for (DWORD i = 0; i < ExportTable.NumberOfNames; ++i) {
		std::string TempFunctionName;

		Done = FALSE;// Reset for next name
		TempFunctionName.clear(); // Empty the string so we can fill it with a new name

		/* Get the function name one character at a time because we don't know how long it is */
		for (UINT_PTR j = 0; !Done; ++j)
		{
			/* Get next character */
			if (!::ReadProcessMemory(hProcessIL2, (LPCVOID)(RemoteModuleBaseVA + ExportNameTable[i] + j),
				&TempChar, sizeof(TempChar), NULL))
				return 0;

			TempFunctionName.push_back(TempChar); // Add it to the string

			/* If it's NUL we are done */
			if (TempChar == (CHAR)'\0')
				Done = TRUE;
		}

		/* Does the name match? */
		if (TempFunctionName.find(functionName) != std::string::npos)
		{
			/* NOTE:
			/* Microsoft's PE/COFF specification says we need to subtract the ordinal base
			/*from the value in the ordinal table but that seems to always give the wrong function */


			/* Make a temporary variable to hold return value*/
			FARPROC TempReturn;

			/* NOTE:
			/* Microsoft's PE/COFF specification says we need to subtract the ordinal base
			/*from the value in the ordinal table but that seems to always give the wrong function */
			//TempReturn = (FARPROC)(RemoteModuleBaseVA + ExportFunctionTable[ExportOrdinalTable[i] - ExportTable.Base]);

			/* So we do it this way instead */
			LPCVOID v0 = (LPCVOID)RemoteModuleBaseVA;
			LPCVOID v1 = (LPCVOID)ExportFunctionTable[ExportOrdinalTable[i]];
			LPCVOID functionAddress = (LPCVOID)((uintptr_t)v0 + (uintptr_t)v1);


			/* Give back that memory */
			delete[] ExportFunctionTable;
			delete[] ExportNameTable;
			delete[] ExportOrdinalTable;

			/* Success!!! */
			return functionAddress;
		}
		/* Wrong function let's try the next... */
	}




	//look at how simpler example uses the tables


	//put write protections back to what they were before we injected
	VirtualProtectEx(hProcessIL2, pStart, moduleRSE.modBaseSize, dwOld, &dwOld);

	//return the start of our allocated memory struct

}