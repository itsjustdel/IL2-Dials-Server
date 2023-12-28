#pragma once
#include <Windows.h>
#include <vector>
#include <string>

std::vector<float> Manifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2, std::string planeType);

std::vector<double> GermanManifolds(LPVOID codeCaveAddress, HANDLE hProcessIL2);

std::vector<double> USManifolds(LPVOID codeCaveAddress, HANDLE hProcess, std::string planeType);

std::vector<double> UKManifolds(LPVOID codeCaveAddress, HANDLE hProcess, std::string planeType);

std::vector<double> RUManifolds(LPVOID codeCaveAddress, HANDLE hProcess, std::string planeType);

bool HookManifold(HANDLE hProcess, void* pSrc, size_t size, LPVOID codeCaveAddress);
