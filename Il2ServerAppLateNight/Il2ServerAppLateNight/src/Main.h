#pragma once
#include <Windows.h>
#include <vector>
//for external access


extern "C" __declspec(dllexport) int InjectCockpitInstruments();

extern "C" __declspec(dllexport) bool GetInjectedCockpit();

extern "C" __declspec(dllexport) intptr_t AOBScanCockpitInstruments();

extern "C" __declspec(dllexport) void GetProcessData();

extern "C" __declspec(dllexport) intptr_t GetAOBResultCockpitInstruments();

extern "C" __declspec(dllexport) void Instruments();

int ReadData(System::ComponentModel::BackgroundWorker^);

double GetAltitude();

double GetMMHg();

double GetAirspeedFromCockpitStruct();

bool ReadCockpitInstruments();

bool ReadAltimeter();


