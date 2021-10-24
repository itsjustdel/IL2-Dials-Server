#pragma once
#include <Windows.h>
#include "USPlanes.h"
#include <string>

uintptr_t OffsetToTurnNeedle(std::string planeType)
{


	//unique offsets for some planes
	if (planeType.compare("P-47D-28") == 0)
		return 0XCA8;

	if (planeType.compare("Yak-7B ser.36") == 0)			
		return 0xC50;	

	if (planeType.compare("Tempest Mk.V ser.2") == 0)
		return 0XCE8;

	if (planeType.compare("Spitfire Mk.IXe") == 0)
		return 0XD10;

	if (IsUSPlane(planeType))
	{


		if (IsTwoEngine(planeType))
			return 0xD78;
		else
			return 0xCF0;


	}

	//default
	return 0xAF0;
}

