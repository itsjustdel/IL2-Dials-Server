#pragma once
//nclude <Windows.h>
//#include <vector>
//for external access

void CaveRecovered();

int InjectCockpitInstruments();

bool GetInjectedCockpit();

intptr_t AOBScanCockpitInstruments();

void GetProcessData();

intptr_t GetAOBResultCockpitInstruments();

int Injector(System::ComponentModel::BackgroundWorker^);

double GetAltitude();

double GetMMHg();

double GetAirspeedFromCockpitStruct();

bool ReadCockpitInstruments();

bool ReadAltimeter();


