#pragma once
#include <Windows.h>
#include "USPlanes.h"
#include <string>
#include "Injector.h"

uintptr_t OffsetToTurnNeedle(std::string planeType)
{
	uintptr_t offset = 0;
	//unique offsets for some planes
	if (planeType.compare("Mosquito F.B. Mk.VI ser.2") == 0)
		offset = 0XDC8;//RSE.RSE::CAeroplane_Mosquito_FB_Mk_VI_s2::simulation+4202 - F2 0F10 0D CED67B00   - movsd xmm1,[RSE.RSE::CTogglesPanel::`vftable'+CD20] { (-40.00) }

	if (planeType.compare("P-47D-28") == 0)
		offset = 0XCA8;

	else if (planeType.compare("Yak-7B ser.36") == 0)
		offset = 0xC50;

	else if (planeType.compare("Tempest Mk.V ser.2") == 0)
		offset = 0XCE8;

	else if (planeType.compare("Spitfire Mk.IXe") == 0)
		offset = 0XD10;

	else if (planeType.compare("Hurricane Mk.II") == 0)
		offset = 0XCE0;

	//yak9 s1 0xC48
	else if (planeType.compare("Yak-9 ser.1") == 0)
		offset = 0XC48;

	//9t 0xC50
	else if (planeType.compare("Yak-9T ser.1") == 0)
		offset = 0XC50;

	//hs 129 = 0xCA8
	else if (planeType.compare("Hs 129 B-2") == 0)
		offset = 0XCA8;

	//Typhoon Mk.Ib	
	else if (planeType.compare("Typhoon Mk.Ib") == 0)
		offset = 0XD48;

	//"U-2VS"
	else if (planeType.compare("U-2VS") == 0)
		offset = 0X10B8;

	//P-47D-22
	else if (planeType.compare("P-47D-22") == 0)
		offset = 0XCA0;

	//Spitfire Mk.XIV
	else if (planeType.compare("Spitfire Mk.XIV") == 0)
		offset = 0XE30;

	//P-51B-5
	else if (planeType.compare("P-51B-5") == 0)
		offset = 0XCD8;

	//grab the a-20 here before us plane check - it has default
	else if (planeType.compare("A-20B") == 0)
		offset = 0xAF0;

	//p38
	//"P-38J-25";
	else if (planeType.compare("P-38J-25") == 0)
	{
		offset = 0xDB0;
		return offset;
	}

	else if (planeType.compare("P-51D-15") == 0)
		offset = 0xCE0;

	else if (IsUSPlane(planeType))
	{
		offset = 0xD28; //patch was +38
		return offset;
	}
	else
	{
		//default
		offset = 0xAF0;
	}
	
	//patch
	offset += 0x38;

	return offset;
}


//bools used for turn needle scan  -move function to turn needle class
bool foundNegativeTurnLimit[100];//oversized
bool foundPositiveTurnLimit[100];
LPCVOID positiveTurnNeedleOffset[100];
LPCVOID negativeTurnNeedleOffset[100];

LPCVOID TurnNeedleScanner(LPCVOID structStart, HANDLE hProcess, bool injectedTurnNeedle, LPVOID codeCaveAddress, HANDLE hProcessIL2)
{
	//we need to wait for injection before we do this
	if (!injectedTurnNeedle)
		return 0;



	//find where to setart our search
	//turn needle location stored at codecave + 140
	LPVOID turnNeedleAddressInCave = (LPVOID)((uintptr_t)(codeCaveAddress)+0x140);
	//read address in cave 
	LPVOID toDynamicBodyStruct = PointerToDataStruct(hProcessIL2, turnNeedleAddressInCave);

	//only search for limits between 20 and 50
	for (size_t a = 10; a < 60; a++)
	{

		//scan through segment of memory we know value is in until we find "limit" 
		//limit is the known value where the rotation is locked - user will fly plane in a manner to max out needle movement while we scan
		for (size_t i = 0; i < 1000 * sizeof(double); i += sizeof(double))
		{
			//add address using uintptr_t and cast back to lpcvoid to use with ReadProcessMemory function		
			LPCVOID targetAddress = (LPCVOID)((uintptr_t)(toDynamicBodyStruct)+i);
			//read a double in to this variable
			double d;
			//for debug to see if we read anything
			size_t bytesRead;
			//read memory from process at "struct start", read a double
			ReadProcessMemory(hProcess, targetAddress, &d, sizeof(double), &bytesRead);

			//check read double against passed "limit"
			//make sure to compare the same data type (negative numbers can differ)

			double doubleA = (double)(a);
			double minusDoubleA = -(double)(a);
			if (d == doubleA)
			{
				//we have found potentially found our offset
				foundPositiveTurnLimit[a] = true;
				positiveTurnNeedleOffset[a] = (LPCVOID)i;
			}

			if (d == minusDoubleA)
			{
				//we have found potentially found our offset
				foundNegativeTurnLimit[a] = true;
				negativeTurnNeedleOffset[a] = (LPCVOID)i;
			}
		}

		//check if we have found both neative and positive offsets
		if (foundNegativeTurnLimit[a] && foundPositiveTurnLimit[a])
		{
			if (negativeTurnNeedleOffset[a] == positiveTurnNeedleOffset[a])
			{
				//looks like we have it!
				return negativeTurnNeedleOffset[a];
			}

		}
	}
	return 0;

}

