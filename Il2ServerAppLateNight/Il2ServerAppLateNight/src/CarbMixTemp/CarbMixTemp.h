#pragma once

#include <windows.h>
#include <string>
#include <vector>

std::vector<float> CarbMixTemps(LPVOID codeCaveAddress, HANDLE hProcess, std::string planeType);
