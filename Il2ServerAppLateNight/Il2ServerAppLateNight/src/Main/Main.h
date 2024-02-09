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

void UpdateTurnNeedle();

bool ReadTurnCoordinatorBall();

bool ReadManifolds();

bool ReadEngineModification();


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

float GetOilTempOut(int engine);

double GetOilTempIn(int engine);

void UpdateWaterTempValues();

void UpdateOilTempValues();

void UpdateCylinderHeadTemps();

double GetCylinderHeadTemp(int engine);

void UpdateCarbMixTemps();

float GetCarbMixTemp(int engine);
