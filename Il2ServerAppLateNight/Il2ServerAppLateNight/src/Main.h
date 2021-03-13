#pragma once
//nclude <Windows.h>
//#include <vector>
//for external access


int InjectCockpitInstruments();

bool GetInjectedCockpit();

intptr_t AOBScanCockpitInstruments();

void GetProcessData();

intptr_t GetAOBResultCockpitInstruments();

int ReadData(System::ComponentModel::BackgroundWorker^);

double GetAltitude();

double GetMMHg();

double GetAirspeedFromCockpitStruct();

bool ReadCockpitInstruments();

bool ReadAltimeter();


