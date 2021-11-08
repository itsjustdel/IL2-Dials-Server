#pragma once

void CaveRecovered();

void GetProcessData();

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

double GetHeading();

double GetPitch();

double GetRoll();

double GetYaw();

double GetVSI();

double GetTurnAndBankBall();

double GetTurnAndBankNeedle();

double GetRPM();





