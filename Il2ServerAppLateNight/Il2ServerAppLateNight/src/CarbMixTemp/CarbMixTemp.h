#pragma once

#include <windows.h>
#include <string>
#include <vector>

std::vector<double> CarbMixTemps(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeName);
