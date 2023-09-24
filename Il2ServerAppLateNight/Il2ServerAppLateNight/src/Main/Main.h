#pragma once

bool GetInjected();

void CaveRecovered();

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

void ReadWaterTemps();

double GetHeading();

double GetPitch();

double GetRoll();

double GetVSI();

double GetTurnAndBankBall();

double GetTurnAndBankNeedle();

double GetRPM(int engine);

float GetManifold(int engine);

int GetEngineModification();

double GetWaterTemp(int engine);

double GetOilTemp(int engine);

double GetOilTempB(int engine);

void UpdateWaterTempValues();

void UpdateOilTempValues();

void UpdateCylinderHeadTemps();

double GetCylinderHeadTemp(int engine);

void UpdateCarbMixTemps();

double GetCarbMixTemp(int engine);
