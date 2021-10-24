#pragma once
#include <Windows.h>
#include <string>

uintptr_t OffsetToTurnNeedle(std::string planeType)
{
	if (planeType.compare("Yak-7B ser.36") == 0)			
		return 0xC50;	

	if (planeType.compare("Tempest Mk.V ser.2") == 0)
		return 0XCE8;

	if (planeType.compare("Spitfire Mk.IXe") == 0)
		return 0XD10;
}

