#pragma once
#include <Windows.h>
#include <vector>
#include <string>

std::vector<double> GermanManifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2);

std::vector<double> USManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string planeType);

std::vector<double> UKManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string planeType);

std::vector<double> RUManifolds(LPCVOID codeCaveAddress, HANDLE hProcess, std::string planeType);


