#pragma once

#include <windows.h>
#include <string>
#include <vector>

std::vector<float> CarbAirMixTemps(LPVOID codeCaveAddress, HANDLE hProcess, std::string planeType);
