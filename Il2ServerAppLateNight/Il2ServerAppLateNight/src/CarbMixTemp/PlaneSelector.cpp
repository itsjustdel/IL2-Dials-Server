#pragma once
#include <string>

// Helper class for plane name string compare

bool isBF109K4(std::string planeName)
{
	std::string v = "Bf 109 K-4";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isBF110E2(std::string planeName)
{
	std::string v = "Bf-110 E2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isBF110G2(std::string planeName)
{
	std::string v = "Bf-110 G2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isHS129B2(std::string planeName)
{
	std::string v = "Hs 129 B-2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isJU88A4(std::string planeName)
{
	std::string v = "Ju-88 A4";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isJU88C6(std::string planeName)
{
	std::string v = "Ju-88 C6";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isJU52(std::string planeName)
{
	std::string v = "Ju-52/3m g4e";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isHurricane(std::string planeName)
{
	std::string v = "Hurricane Mk.II";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isTempest(std::string planeName)
{
	std::string v = "Tempest Mk.V ser.2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isTyphoon(std::string planeName)
{
	std::string v = "Typhoon Mk.Ib";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isMosquito(std::string planeName)
{
	std::string v = "Mosquito F.B. Mk.VI ser.2";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}
bool isP39(std::string planeName)
{
	std::string v = "P-39L-1";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isP47D22(std::string planeName)
{
	std::string v = "P-47D-22";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isP47D28(std::string planeName)
{
	std::string v = "P-47D-28";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isP51B5(std::string planeName)
{
	std::string v = "P-51B-5";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isP51D15(std::string planeName)
{
	std::string v = "P-51D-15";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}
bool isP38J(std::string planeName)
{
	std::string v = "P-38J-25";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}
bool isA20B(std::string planeName)
{
	std::string v = "A-20B";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}

bool isC47(std::string planeName)
{
	std::string v = "C-47A";
	if (planeName.compare(v) == 0)
		return true;

	return false;
}