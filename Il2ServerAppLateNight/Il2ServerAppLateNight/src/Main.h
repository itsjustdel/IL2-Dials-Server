#pragma once

bool GetInjected();

void CaveRecovered();

HANDLE GetIL2Handle();

LPVOID GetCodeCaveAddress();

//void GetProcessData();

int Injector(System::ComponentModel::BackgroundWorker^);

double GetAltitude();

double GetMMHg();

double GetAirspeedFromCockpitStruct();

float GetIL2DialsVersion();

std::string GetPlaneType();

bool ReadPlaneType();

bool ReadCockpitInstruments();

bool ReadAltimeter();

bool ReadTurnNeedle();

bool ReadTurnCoordinatorBall();

bool ReadManifolds();

bool ReadEngineModification();

double GetHeading();

double GetPitch();

double GetRoll();

//double GetYaw();

double GetVSI();

double GetTurnAndBankBall();

double GetTurnAndBankNeedle();

double GetRPM(int engine);

double GetManifold(int engine);

int GetEngineModification();

double GetWaterTemp(int engine);





